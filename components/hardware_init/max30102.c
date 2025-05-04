#include "max30102.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/task.h"
#include <string.h> // for memset

#define MAX30102_ADDR 0x57
#define I2C_MASTER_TIMEOUT_MS 1000
#define I2C_PORT I2C_NUM_0
#define MAX30102_SAMPLE_COUNT 160


static const char *TAG = "MAX30102";

static uint8_t last_heart_rate = 0;

static uint32_t red_buffer[MAX30102_SAMPLE_COUNT];
static uint32_t ir_buffer[MAX30102_SAMPLE_COUNT];

// 寫入暫存器
esp_err_t max30102_write_reg(uint8_t reg, uint8_t value) {
    return i2c_master_write_to_device(I2C_PORT, MAX30102_ADDR,
        (uint8_t[]){reg, value}, 2, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
}

// 讀取暫存器
esp_err_t max30102_read_reg(uint8_t reg, uint8_t *value) {
    return i2c_master_write_read_device(I2C_PORT, MAX30102_ADDR,
        &reg, 1, value, 1, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
}

// 初始化 MAX30102
esp_err_t max30102_init(void) {

    uint8_t part_id = 0; 
    esp_err_t err;
    
    // 讀取並確認 PART ID
    err = max30102_read_reg(0xFF, &part_id);
    if (err != ESP_OK || part_id != 0x15) {
        ESP_LOGE(TAG, "Invalid MAX30102 ID: 0x%02X", part_id);
        return ESP_FAIL;
    }


    // Reset sensor
    max30102_write_reg(0x09, 0x40); // RESET
    vTaskDelay(pdMS_TO_TICKS(100));

    // FIFO Configuration
    max30102_write_reg(0x08, 0x0F); // sample avg = 4, FIFO rollover enabled, almost full = 17

    // Mode Configuration
    max30102_write_reg(0x09, 0x03); // SPO2 mode

    // SPO2 Configuration
    max30102_write_reg(0x0A, 0x27); // 100Hz sample rate, 411us pulse width

    // LED Pulse Amplitude
    max30102_write_reg(0x0C, 0x24); // RED
    max30102_write_reg(0x0D, 0x24); // IR

    // 清空 FIFO
    max30102_write_reg(0x04, 0x00); // write pointer
    max30102_write_reg(0x05, 0x00); // overflow counter
    max30102_write_reg(0x06, 0x00); // read pointer

    memset(red_buffer, 0, sizeof(red_buffer));
    memset(ir_buffer, 0, sizeof(ir_buffer));

    ESP_LOGI(TAG, "MAX30102 initialized");
    return ESP_OK;
}

// 讀取 FIFO 中的單筆樣本
static esp_err_t max30102_read_fifo(uint32_t *red, uint32_t *ir) {
    uint8_t data[6];
    esp_err_t ret = i2c_master_write_read_device(I2C_PORT, MAX30102_ADDR,
        (uint8_t[]){0x07}, 1, data, 6, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
    
    if (ret == ESP_OK) {
    *ir  = ((uint32_t)(data[0] & 0x03) << 16) | ((uint32_t)data[1] << 8) | data[2];
    *red = ((uint32_t)(data[3] & 0x03) << 16) | ((uint32_t)data[4] << 8) | data[5];        
    }
    return ret;
}

#define THRESHOLD_FACTOR 1.1f
#define MIN_PEAK_DISTANCE 20  // 最小間隔 samples，對應約 180 bpm


//心跳不會高於 180 bpm → 最小間隔 ≈ 0.33 秒 ≈ 33 samples
//設定兩個波峰之間至少相隔 30 sampl
static int detect_peaks(uint32_t *ir_data, int count, int sampling_rate_hz) {
    int peak_count = 0;
    uint32_t sum = 0;
    for (int i = 0; i < count; i++) {
        sum += ir_data[i];
    }
    float mean = sum / (float)count;

    int last_peak_index = -MIN_PEAK_DISTANCE; // 初始化為足夠小，確保第一個 peak 不會被忽略

    for (int i = 1; i < count - 1; i++) {
        if (ir_data[i] > ir_data[i - 1] &&
            ir_data[i] > ir_data[i + 1] &&
            ir_data[i] > mean * THRESHOLD_FACTOR &&
            (i - last_peak_index) >= MIN_PEAK_DISTANCE) {

            peak_count++;
            last_peak_index = i;
        }
    }

    // bpm = peaks / seconds * 60
    float seconds = (float)count / sampling_rate_hz;
    int bpm = (int)((peak_count / seconds) * 60.0f);
    return bpm;
}

//平均濾波法，窗口5(取5數據左右各2)
static void moving_average_filter(const uint32_t *input, uint32_t *output, int len, int window_size) {
    int half_window = window_size / 2;

    for (int i = 0; i < len; i++) {
        uint64_t sum = 0;
        int count = 0;

        for (int j = -half_window; j <= half_window; j++) {
            int idx = i + j;
            if (idx >= 0 && idx < len) {
                sum += input[idx];
                count++;
            }
        }
        output[i] = sum / count;
    }
}


// 簡單平均算法：HR 與 SpO₂ 為紅/IR 平均比值
static void max30102_process_data(uint8_t *heart_rate, uint8_t *spo2) {
    uint64_t red_sum = 0, ir_sum = 0;

    for (int i = 0; i < MAX30102_SAMPLE_COUNT; i++) {
        red_sum += red_buffer[i];
        ir_sum  += ir_buffer[i];
    }

    uint32_t red_avg = red_sum / MAX30102_SAMPLE_COUNT;
    uint32_t ir_avg  = ir_sum / MAX30102_SAMPLE_COUNT;

    // SpO2 模擬算法
    float ratio = (float)red_avg / (float)(ir_avg + 1);
    float spo2_f = 104 - (17 * ratio);
    if (spo2_f > 100) spo2_f = 100;
    if (spo2_f < 90)  spo2_f = 90;
    *spo2 = (uint8_t)spo2_f;

    // 平滑處理
    uint32_t filtered_ir[MAX30102_SAMPLE_COUNT];
    moving_average_filter(ir_buffer, filtered_ir, MAX30102_SAMPLE_COUNT, 5); // 使用窗口 5

    // 偵測波峰 → 心率
    int bpm = detect_peaks(filtered_ir, MAX30102_SAMPLE_COUNT, 100); // 100Hz 採樣率
    if (bpm < 20 || bpm > 200) {
        bpm = 0;
    }else{
        last_heart_rate = bpm;
    }
    *heart_rate = bpm;
}

// 提供公開呼叫，內部讀取 32 筆資料後估算
esp_err_t max30102_read(uint8_t *heart_rate, uint8_t *spo2) {
    for (int i = 0; i < MAX30102_SAMPLE_COUNT; i++) {
        if (max30102_read_fifo(&red_buffer[i], &ir_buffer[i]) != ESP_OK) {
            return ESP_FAIL;
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // 100Hz
    }

    // 濾波處理
    uint32_t filtered_ir[MAX30102_SAMPLE_COUNT];
    moving_average_filter(ir_buffer, filtered_ir, MAX30102_SAMPLE_COUNT, 5); // 平滑用5筆

    // 輸出原始與濾波 IR 數據
    for (int i = 0; i < MAX30102_SAMPLE_COUNT; i++) {
        printf("%lu,%lu\n", ir_buffer[i], filtered_ir[i]);  // 格式：原始,濾波
    }

    // 資料處理（心率與SpO2估算）
    max30102_process_data(heart_rate, spo2);

    return ESP_OK;
}


#include "sensor_display.h"
#include "aht10.h"
#include "max30102.h"
#include "screens.h"         
#include "esp_log.h"
#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "SensorDisplay";

static TaskHandle_t sensor_task_handle = NULL;

extern bool lvgl_lock(int timeout_ms);
extern int lvgl_unlock();

void sensor_display_update(void) {
    float temperature = 0.0f, humidity = 0.0f;
    uint8_t heart_rate = 0, spo2 = 0;

    esp_err_t aht_status = aht10_read_data(&temperature, &humidity);
    esp_err_t max_status = max30102_read(&heart_rate, &spo2);

    if (aht_status == ESP_OK) {
        ESP_LOGI("aht10","t: %.1f",temperature);
        ESP_LOGI("aht10","h: %.1f",humidity);
        if (lvgl_lock(-1)) {
            if (objects.temp_label) {
                char temp_str[33];
                int temp_int = (int)temperature;
                int temp_frac = (int)((temperature - temp_int) * 10.0f);  // 保留1位小數
            
                // 處理負數的小數部分
                if (temp_frac < 0) temp_frac = -temp_frac;
            
                snprintf(temp_str, sizeof(temp_str), "Temp: %d.%d °C", temp_int, temp_frac);
                lv_label_set_text_fmt(objects.temp_label, temp_str);
            }
            if (objects.humi_label) {
                char humi_str[33];
                int humi_int = (int)humidity;
                int humi_frac = (int)((humidity - humi_int) * 10.0f);
                if (humi_frac < 0) humi_frac = -humi_frac;
                
                snprintf(humi_str, sizeof(humi_str), "Humi: %d.%d %%", humi_int, humi_frac);
                lv_label_set_text(objects.humi_label, humi_str);
            }
            lvgl_unlock();
        }
    
    } else {
      ESP_LOGW(TAG, "AHT10 read failed");
      lv_label_set_text_fmt(objects.temp_label, "Temp:-- °C");
      lv_label_set_text_fmt(objects.humi_label, "Humi:-- %%");
    }

    if (max_status == ESP_OK && heart_rate != 0) {
        lv_label_set_text_fmt(objects.bpm_label, "BPM:%d bpm", heart_rate);
        lv_label_set_text_fmt(objects.spo2_label, "SpO2:%d %%", spo2);
    } else {
        ESP_LOGW(TAG, "MAX30102 read failed");
        lv_label_set_text_fmt(objects.bpm_label, "BPM:-- bpm");
        lv_label_set_text_fmt(objects.spo2_label, "SpO2:-- %%");
    }
}

static void sensor_display_task(void *param) {
    while (1) {
        sensor_display_update();
        vTaskDelay(pdMS_TO_TICKS(5000)); // 每5秒更新
    }
}

void sensor_display_start(void) {
    if (sensor_task_handle == NULL) {
        xTaskCreate(sensor_display_task, "sensor_display_task", 4096, NULL, 5, &sensor_task_handle);
        ESP_LOGI(TAG, "Sensor display task started");
    }
}

void sensor_display_stop(void) {
    if (sensor_task_handle) {
        vTaskDelete(sensor_task_handle);
        sensor_task_handle = NULL;
        ESP_LOGI(TAG, "Sensor display task stopped");
    }
}
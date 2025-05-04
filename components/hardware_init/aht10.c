#include "aht10.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/task.h"

#define AHT10_ADDR 0x38
#define I2C_PORT I2C_NUM_0

static const char *TAG = "AHT10";

esp_err_t aht10_init(void) {
    uint8_t init_cmd[3] = {0xE1, 0x08, 0x00};
    esp_err_t err = i2c_master_write_to_device(I2C_PORT, AHT10_ADDR, init_cmd, sizeof(init_cmd), pdMS_TO_TICKS(100));

    if (err == ESP_OK) {
        ESP_LOGI("AHT10", "Initialization command sent");
    } else {
        ESP_LOGE("AHT10", "Init failed: %s", esp_err_to_name(err));
    }
    return err;
}

esp_err_t aht10_read_data(float *temperature, float *humidity) {
    uint8_t trigger_cmd[1] = {0xAC};
    uint8_t dummy[2] = {0x33, 0x00};
    i2c_master_write_to_device(I2C_PORT, AHT10_ADDR, trigger_cmd, 1, pdMS_TO_TICKS(20));
    i2c_master_write_to_device(I2C_PORT, AHT10_ADDR, dummy, 2, pdMS_TO_TICKS(20));
    vTaskDelay(pdMS_TO_TICKS(80));

    uint8_t data[6];
    esp_err_t err = i2c_master_read_from_device(I2C_PORT, AHT10_ADDR, data, 6, pdMS_TO_TICKS(100));
    if (err != ESP_OK) return err;

    uint32_t raw_hum = ((uint32_t)(data[1]) << 12) | ((uint32_t)(data[2]) << 4) | (data[3] >> 4);
    uint32_t raw_temp = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)(data[4]) << 8) | data[5];

    *humidity = (raw_hum * 100.0) / 1048576.0;
    *temperature = (raw_temp * 200.0 / 1048576.0) - 50.0;

    return ESP_OK;
}

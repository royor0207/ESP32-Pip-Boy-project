#include <stdio.h>
#include "hardware_init.h"
#include "screen_switcher.h"
#include "driver/i2c.h"

#define GPIO_SCREEN_0 GPIO_NUM_15
#define GPIO_SCREEN_1 GPIO_NUM_2
#define GPIO_SCREEN_2 GPIO_NUM_0
#define GPIO_SCREEN_3 GPIO_NUM_25
#define GPIO_SCREEN_4 GPIO_NUM_3

#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_SDA_IO GPIO_NUM_21
#define I2C_MASTER_SCL_IO GPIO_NUM_22
#define I2C_MASTER_FREQ_HZ 100000


esp_err_t i2c_master_init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 21,
        .scl_io_num = 22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };
    esp_err_t err;

    err = i2c_param_config(I2C_NUM_0, &conf);
    if (err != ESP_OK) {
        ESP_LOGE("I2C", "i2c_param_config failed: %s", esp_err_to_name(err));
        return err;
    }

    err = i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
    if (err != ESP_OK) {
        ESP_LOGE("I2C", "Driver install failed: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI("I2C", "Driver install OK");
    }

    return err;
}

void hardware_init(void) {

    encoder_init();

    // 初始化感測器（I2C）
    i2c_master_init();

    aht10_init();

    // 初始化音樂播放器 UART
    music_player_init();

    vTaskDelay(pdMS_TO_TICKS(200));

    if (max30102_init() != ESP_OK) {
        ESP_LOGE("I2C", "MAX30102 init failed");
    }





    // 初始化五段切換開關 GPIO 為輸入並啟用中斷
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_NEGEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << GPIO_SCREEN_0) |
                        (1ULL << GPIO_SCREEN_1) |
                        (1ULL << GPIO_SCREEN_2) |
                        (1ULL << GPIO_SCREEN_3) |
                        (1ULL << GPIO_SCREEN_4),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE
    };
    gpio_config(&io_conf);



    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_SCREEN_0, screen_gpio_isr_handler, (void*)GPIO_SCREEN_0);
    gpio_isr_handler_add(GPIO_SCREEN_1, screen_gpio_isr_handler, (void*)GPIO_SCREEN_1);
    gpio_isr_handler_add(GPIO_SCREEN_2, screen_gpio_isr_handler, (void*)GPIO_SCREEN_2);
    gpio_isr_handler_add(GPIO_SCREEN_3, screen_gpio_isr_handler, (void*)GPIO_SCREEN_3);
    gpio_isr_handler_add(GPIO_SCREEN_4, screen_gpio_isr_handler, (void*)GPIO_SCREEN_4);

    
    ESP_LOGI("Hardware", "All hardware initialized.");
}
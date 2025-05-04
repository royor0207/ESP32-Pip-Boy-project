#include <stdio.h>
#include "encoder.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ENC1_CLK GPIO_NUM_12
#define ENC1_DT  GPIO_NUM_34
#define ENC1_SW  GPIO_NUM_35
#define ENC2_CLK GPIO_NUM_5
#define ENC2_DT  GPIO_NUM_36
#define ENC2_SW  NULL

#define DEBOUNCE_DELAY_MS 50  // 消抖延遲時間（毫秒）

static void pcnt_init_encoder(pcnt_unit_t unit, gpio_num_t pulse_gpio, gpio_num_t ctrl_gpio) {
    pcnt_config_t pcnt_config = {
        .pulse_gpio_num = pulse_gpio,
        .ctrl_gpio_num = ctrl_gpio,
        .channel = PCNT_CHANNEL_0,
        .unit = unit,
        .pos_mode = PCNT_COUNT_INC,
        .neg_mode = PCNT_COUNT_DEC,
        .lctrl_mode = PCNT_MODE_REVERSE,
        .hctrl_mode = PCNT_MODE_KEEP,
        .counter_h_lim = 10,
        .counter_l_lim = -10
    };
    pcnt_unit_config(&pcnt_config);
    pcnt_counter_pause(unit);
    pcnt_counter_clear(unit);
    pcnt_counter_resume(unit);
}

void encoder_init(void) {
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
        .pin_bit_mask = (1ULL << ENC1_SW) //| (1ULL << ENC2_SW)
    };
    gpio_config(&io_conf);

    pcnt_init_encoder(PCNT_UNIT_0, ENC1_CLK, ENC1_DT);
    pcnt_init_encoder(PCNT_UNIT_1, ENC2_CLK, ENC2_DT);
    ESP_LOGI("ENCODER","encoder init");
}

int read_and_clear_encoder_pcnt(int encoder_num) {
    pcnt_unit_t unit = (encoder_num == 0) ? PCNT_UNIT_0 : PCNT_UNIT_1;
    int16_t count = 0;
    pcnt_get_counter_value(unit, &count);
    if (count != 0) pcnt_counter_clear(unit);
    return count;
}



int is_encoder_button_pressed(int encoder_num) {
    // 設定對應的按鈕腳位
    gpio_num_t pin = ENC1_SW;  // 可以根據 encoder_num 決定是 ENC1_SW 還是 ENC2_SW

    // 讀取按鈕狀態
    int button_state = gpio_get_level(pin);

    // 若按鈕狀態為低，則進行消抖
    if (button_state == 0) {
        // 延遲一定時間以消除按鈕彈跳
        vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_DELAY_MS));

        // 再次檢查按鈕狀態
        if (gpio_get_level(pin) == 0) {
            return 1;  // 按鈕確實被按下
        }
    }

    return 0;  // 按鈕未被按下
}

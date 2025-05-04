#include <stdio.h>
#include "screen_switcher.h"
#include "hardware_init.h"
#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "time_display.h"
#include "music_screen.h"
#include "sensor_display.h"
#include "tetris_game.h"
#include "setting_display.h"

static const struct {
    int gpio;
    screen_id_t screen;
    uint32_t interval_ms;
} gpio_screen_map[] = {
    { GPIO_SCREEN_0, SCREEN_ID_MAIN,    1000 },
    { GPIO_SCREEN_1, SCREEN_ID_SENSOR,   500 },
    { GPIO_SCREEN_2, SCREEN_ID_MUSIC,    300 },
    { GPIO_SCREEN_3, SCREEN_ID_GAME,   50  },
    { GPIO_SCREEN_4, SCREEN_ID_SETTING,  500 }
};

static uint32_t current_interval = 1000;
static screen_id_t current_screen = SCREEN_ID_MAIN;

QueueHandle_t screen_queue = NULL;


void screen_switch_to(screen_id_t new_screen) {

    ESP_LOGI("screen_switcher", "Switching screen from %d to %d", current_screen, new_screen);

    // 停止目前畫面任務
    switch (current_screen) {
        case SCREEN_ID_MAIN:
            time_display_stop();
            break;
        case SCREEN_ID_SENSOR:
            sensor_display_stop();
            break;
        case SCREEN_ID_MUSIC:
            music_screen_stop();
            break;
        case SCREEN_ID_GAME:
            tetris_game_stop();
            break;
        case SCREEN_ID_SETTING:
            setting_task_stop();
            break;
        default:
            break;
    }

    switch (new_screen) {
        case SCREEN_ID_MAIN:
            lv_scr_load_anim(objects.main, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
            time_display_start();            
            break;
        case SCREEN_ID_SENSOR:
            lv_scr_load_anim(objects.sensor, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
            sensor_display_start();
            break;
        case SCREEN_ID_MUSIC:
            lv_scr_load_anim(objects.music, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
            music_screen_start();
            break;
        case SCREEN_ID_GAME:
            lv_scr_load_anim(objects.game, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
            tetris_game_start();
            break;
        case SCREEN_ID_SETTING:
            lv_scr_load_anim(objects.setting, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
            setting_task_start();
            break;
        default:
            if (new_screen < SCREEN_ID_MAIN || new_screen > SCREEN_ID_SETTING) {
                ESP_LOGW("screen_switcher", "Invalid screen id received: %d", new_screen);
            }     
            return;
    }

    ESP_LOGI("screen_switcher", "Switching screen from %d to %d", current_screen, new_screen);


    // 觸發 EEZ Studio 畫面更新
    tick_screen_by_id(new_screen);

    // 更新畫面
    current_screen = new_screen;

    //停用回傳與更新間隔
    //for (int i = 0; i < sizeof(gpio_screen_map) / sizeof(gpio_screen_map[0]); i++) {
    //    if (gpio_screen_map[i].screen == new_screen) {
    //        current_interval = gpio_screen_map[i].interval_ms;
    //        break;
    //    }
    //}
}

screen_id_t screen_switcher_get_current(void) {
    return current_screen;
}

//uint32_t screen_task_get_interval(void) {
//    return current_interval;
//}

static uint32_t last_isr_ticks = 0;
#define DEBOUNCE_TICKS pdMS_TO_TICKS(100)

void IRAM_ATTR screen_gpio_isr_handler(void* arg) {
    if (screen_queue == NULL) return;

    //消抖動
    uint32_t now = xTaskGetTickCountFromISR();
    if (now - last_isr_ticks < DEBOUNCE_TICKS) return;
    last_isr_ticks = now;

    int gpio_num = (int)(intptr_t)arg;
    ESP_EARLY_LOGI("ISR", "Interrupt on GPIO %d", gpio_num);

    screen_id_t screen = SCREEN_ID_MAIN;
    bool found = false;

    for (int i = 0; i < sizeof(gpio_screen_map) / sizeof(gpio_screen_map[0]); i++) {
        if (gpio_num == gpio_screen_map[i].gpio) {
            screen = gpio_screen_map[i].screen;
            found = true;
            break;
        }
    }

    if (!found) {
        ESP_EARLY_LOGI("ISR", "Screen mapping not found for GPIO %d", gpio_num);
        return;  // 沒找到對應畫面，直接略過
    }
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(screen_queue, &screen, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}



void screen_switcher_task(void *arg) {
    screen_id_t screen;
    while (1) {
        if (xQueueReceive(screen_queue, &screen, portMAX_DELAY)) {
            if (screen != current_screen) {
                screen_switch_to(screen);
            }
        }
    }
}

void screen_switcher_start_task() {
    xTaskCreate(screen_switcher_task, "screen_switcher_task", 4096, NULL, 5, NULL);
}



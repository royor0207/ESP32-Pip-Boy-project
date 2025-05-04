#ifndef SCREEN_SWITCHER_H
#define SCREEN_SWITCHER_H

#include "lvgl.h"
#include "screens.h"
#include "freertos/FreeRTOS.h"
 #include "freertos/queue.h"

typedef enum ScreensEnum screen_id_t;

extern QueueHandle_t screen_queue;

// 中斷 傳入隊列 任務讀取隊列信息 執行畫面切換
void screen_switcher_start_task();


screen_id_t screen_switcher_get_current(void);

// 回傳目前畫面對應的任務執行間隔（ms）
//uint32_t screen_task_get_interval(void);

void screen_gpio_isr_handler(void* arg);

#endif // SCREEN_SWITCHER_H

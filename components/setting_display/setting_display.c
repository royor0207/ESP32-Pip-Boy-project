#include <stdio.h>
#include "setting_display.h"
#include "esp_log.h"
#include "lvgl.h"
#include "encoder.h"
#include "music_screen.h"
#include "screens.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define NUM_SETTING 1

static int32_t new_value = 10;


static bool setting_objects_initialized = false;

static lv_obj_t *setting_labels[NUM_SETTING] ;
static lv_obj_t *setting_sliders[NUM_SETTING] ;

void setting_ui_init(void) {
    if (!setting_objects_initialized) {
    setting_labels[0] = objects.volume_label;
    setting_sliders[0] = objects.volume_slider;
    setting_objects_initialized = true;
    }
}



static int current_selection = 0;
static bool is_changing = false;

static void update_setting_states(void) {
    for (int i = 0; i < NUM_SETTING ; ++i) {
        lv_obj_clear_state(setting_labels[i], LV_STATE_CHECKED);
        lv_obj_clear_state(setting_sliders[i], LV_STATE_CHECKED);

    }
    lv_obj_add_state(setting_labels[current_selection], LV_STATE_CHECKED);
    lv_obj_add_state(setting_sliders[current_selection], LV_STATE_CHECKED);
}


//第一個編碼器切換設定項
void handle_encoder_navigation(void) {
   int delta = read_and_clear_encoder_pcnt(1);
   if (delta != 0) {
       current_selection += delta;
       if (current_selection < 0) current_selection = NUM_SETTING-1 ;
       if (current_selection > NUM_SETTING-1 ) current_selection = 0;
       update_setting_states();
   }
}

static void handle_encoder_press(void) {
    if (is_encoder_button_pressed(0)) {
        if (is_changing) {
            music_screen_set_volume(new_value);
            is_changing = false;
            return;
        }        
        is_changing = true;
    }
}

void handle_encoder2_navigation(void) {
    if (is_changing != 0) {
        int32_t encoder_delta = read_and_clear_encoder_pcnt(0);  // 取得旋轉差值（正負）
        if (encoder_delta != 0) {
            int32_t cur_value = lv_slider_get_value(setting_sliders[current_selection]);
            new_value = cur_value + encoder_delta;

            // 限制在範圍內
            int32_t min = 0;
            int32_t max = 100;
            if (new_value < min) new_value = min;
            if (new_value > max) new_value = max;

            lv_slider_set_value(setting_sliders[current_selection], new_value, LV_ANIM_OFF);
        }
    }
}

void volume_setting_task(void *param) {
    //int32_t last_encoder_value = 0;

    while (1) {
        handle_encoder_navigation();
        handle_encoder_press();
        handle_encoder2_navigation();
        vTaskDelay(pdMS_TO_TICKS(50));  // 根據需要調整更新頻率
    }
}


static TaskFunction_t setting_task[NUM_SETTING] = {
    volume_setting_task,
};

static TaskHandle_t setting_task_handle[NUM_SETTING] = {
    NULL ,
};

void setting_task_start (void) {
    setting_ui_init();
    if (setting_task_handle[current_selection] == NULL) {
        xTaskCreate (setting_task[current_selection],"SETTING TASK",2048,NULL,5,&setting_task_handle[current_selection]);
        ESP_LOGI("SETTING TASK","task create");
    }
}

void setting_task_stop(void) {
    if (setting_task_handle[current_selection] != NULL) {
        vTaskDelete(setting_task_handle[current_selection]);
        setting_task_handle[current_selection] = NULL;
        ESP_LOGI("SETTING TASK","task delete");

    }
}


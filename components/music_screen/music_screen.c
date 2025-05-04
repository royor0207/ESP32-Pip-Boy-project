#include "music_screen.h"
#include "screens.h"
#include "esp_log.h"
#include "encoder.h"
#include "music_player.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static TaskHandle_t music_task_handle = NULL;
static int current_selection = 0;
static bool is_playing = false;
static int current_volume = 50;
static int current_playing_music = -1;
static bool music_labels_initialized = false;

#define NUM_MUSIC_LABELS 4
static lv_obj_t* music_labels[NUM_MUSIC_LABELS];

void music_labels_init(void) {
    if (!music_labels_initialized) {
    music_labels[0] = objects.music1_label;
    music_labels[1] = objects.music2_label;
    music_labels[2] = objects.music3_label;
    music_labels[3] = objects.music4_label;
    music_labels_initialized = true;
    }
}

// 音量調整接口
void music_screen_set_volume(int volume) {
    if (volume < 0) volume = 0;
    if (volume > 100) volume = 100;
    current_volume = volume;

    // 若正在播放，立即套用 (未加入換頁繼續播放)
    if (is_playing) {
        music_player_set_volume_percent(current_volume);
    }
}

static void update_label_states(void) {
    for (int i = 0; i < NUM_MUSIC_LABELS; ++i) {
        lv_obj_clear_state(music_labels[i], LV_STATE_CHECKED);
    }
    lv_obj_add_state(music_labels[current_selection], LV_STATE_CHECKED);
}

static void handle_encoder_navigation(void) {
    int delta = read_and_clear_encoder_pcnt(0);
    if (delta != 0) {
        current_selection += delta;
        if (current_selection < 0) current_selection = NUM_MUSIC_LABELS-1;
        if (current_selection > NUM_MUSIC_LABELS-1 ) current_selection = 0;
        update_label_states();
    }
}

static void handle_encoder_press(void) {
    if (is_encoder_button_pressed(0)) {
        music_player_set_volume_percent(current_volume);
        if (is_playing) {
            music_player_pause();
            if (current_playing_music != current_selection) {
                music_player_play(current_selection+1);
                ESP_LOGI("MUSIC PLAYER","playing %d",current_selection);
                current_playing_music = current_selection;
            }else{
                is_playing = false;
            }
        } else {
            if (current_playing_music != current_selection) {
                music_player_play(current_selection+1);
                ESP_LOGI("MUSIC PLAYER","playing %d",current_selection);
                current_playing_music = current_selection;
                is_playing = true;
            }else{
                music_player_resume();
                is_playing = true;
            }
        }
    }
}

static void music_screen_task(void *param) {
    update_label_states();
    while (1) {
        handle_encoder_navigation();
        handle_encoder_press();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void music_screen_start(void) {
    if (music_task_handle == NULL) {
        music_labels_init();
        xTaskCreate(music_screen_task, "music_screen_task", 4096, NULL, 5, &music_task_handle);
        ESP_LOGI("music_screen","start");
    }
}

void music_screen_stop(void) {
    if (music_task_handle) {
        vTaskDelete(music_task_handle);
        music_task_handle = NULL;
        ESP_LOGI("music_screen","stop");

    }
    music_player_pause();
    is_playing = false;
}



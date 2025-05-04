#include "actions.h"
#include "screen_switcher.h"
#include "sensor_display.h"
#include "music_screen.h"
#include "tetris_game.h"
#include "esp_log.h"
#include "time_display.h"
#include "hardware_init.h"
#include "setting_display.h"


void action_time_task(lv_event_t *e) {
    time_display_start();
}

void action_sensor_task(lv_event_t *e) {
    sensor_display_start();
}

void action_music_task(lv_event_t *e) {
    music_screen_start();
}

void action_tetris_task(lv_event_t *e) {
    tetris_game_start();
}

void action_setting_task(lv_event_t *e) {
    setting_task_start();
}

void action_screen_switch_task(lv_event_t *e) {
    //screen_switcher_start_task();
}

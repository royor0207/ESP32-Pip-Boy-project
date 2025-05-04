#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *sensor;
    lv_obj_t *music;
    lv_obj_t *game;
    lv_obj_t *setting;
    lv_obj_t *label_time;
    lv_obj_t *label_date;
    lv_obj_t *spo2_label;
    lv_obj_t *bpm_label;
    lv_obj_t *humi_label;
    lv_obj_t *temp_label;
    lv_obj_t *music1_label;
    lv_obj_t *music2_label;
    lv_obj_t *music4_label;
    lv_obj_t *music3_label;
    lv_obj_t *tetris_score;
    lv_obj_t *start_label;
    lv_obj_t *tetris_canvas;
    lv_obj_t *preview_canvas;
    lv_obj_t *volume_label;
    lv_obj_t *volume_slider;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_SENSOR = 2,
    SCREEN_ID_MUSIC = 3,
    SCREEN_ID_GAME = 4,
    SCREEN_ID_SETTING = 5,
};

void create_screen_main();
void tick_screen_main();

void create_screen_sensor();
void tick_screen_sensor();

void create_screen_music();
void tick_screen_music();

void create_screen_game();
void tick_screen_game();

void create_screen_setting();
void tick_screen_setting();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();




#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/
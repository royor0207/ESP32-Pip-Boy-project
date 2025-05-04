#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_sensor_task(lv_event_t * e);
extern void action_music_task(lv_event_t * e);
extern void action_tetris_task(lv_event_t * e);
extern void action_setting_task(lv_event_t * e);
extern void action_time_task(lv_event_t * e);
extern void action_screen_switch_task(lv_event_t * e);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/
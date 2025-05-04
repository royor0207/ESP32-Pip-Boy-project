#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: CRT_Pipboy_Screen
lv_style_t *get_style_crt_pipboy_screen_MAIN_DEFAULT();
void add_style_crt_pipboy_screen(lv_obj_t *obj);
void remove_style_crt_pipboy_screen(lv_obj_t *obj);

// Style: CRT_Pipboy_label
lv_style_t *get_style_crt_pipboy_label_MAIN_DEFAULT();
lv_style_t *get_style_crt_pipboy_label_MAIN_CHECKED();
void add_style_crt_pipboy_label(lv_obj_t *obj);
void remove_style_crt_pipboy_label(lv_obj_t *obj);

// Style: CRT_Pipboy_Slider
lv_style_t *get_style_crt_pipboy_slider_MAIN_CHECKED();
lv_style_t *get_style_crt_pipboy_slider_MAIN_DEFAULT();
lv_style_t *get_style_crt_pipboy_slider_INDICATOR_DEFAULT();
lv_style_t *get_style_crt_pipboy_slider_INDICATOR_CHECKED();
lv_style_t *get_style_crt_pipboy_slider_KNOB_DEFAULT();
lv_style_t *get_style_crt_pipboy_slider_KNOB_CHECKED();
void add_style_crt_pipboy_slider(lv_obj_t *obj);
void remove_style_crt_pipboy_slider(lv_obj_t *obj);

// Style: CRT_Pipboy_Canvas
lv_style_t *get_style_crt_pipboy_canvas_MAIN_DEFAULT();
void add_style_crt_pipboy_canvas(lv_obj_t *obj);
void remove_style_crt_pipboy_canvas(lv_obj_t *obj);



#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/
#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: CRT_Pipboy_Screen
//

void init_style_crt_pipboy_screen_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_img_opa(style, 40);
    lv_style_set_bg_color(style, lv_color_hex(0xff001a00));
    lv_style_set_bg_img_src(style, &img_scanline);
};

lv_style_t *get_style_crt_pipboy_screen_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_crt_pipboy_screen_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_crt_pipboy_screen(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_crt_pipboy_screen_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_crt_pipboy_screen(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_crt_pipboy_screen_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: CRT_Pipboy_label
//

void init_style_crt_pipboy_label_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(0xff00ff00));
    lv_style_set_text_align(style, LV_TEXT_ALIGN_LEFT);
    lv_style_set_bg_grad_dir(style, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(style, 130);
    lv_style_set_bg_grad_stop(style, 255);
    lv_style_set_bg_main_stop(style, 200);
    lv_style_set_bg_grad_color(style, lv_color_hex(0xff023d0f));
    lv_style_set_bg_color(style, lv_color_hex(0xff001a00));
};

lv_style_t *get_style_crt_pipboy_label_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_crt_pipboy_label_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_crt_pipboy_label_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff00ff00));
    lv_style_set_text_color(style, lv_color_hex(0xff001a00));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_text_opa(style, 255);
};

lv_style_t *get_style_crt_pipboy_label_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_crt_pipboy_label_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_crt_pipboy_label(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_crt_pipboy_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_crt_pipboy_label_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_crt_pipboy_label(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_crt_pipboy_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_crt_pipboy_label_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: CRT_Pipboy_Slider
//

void init_style_crt_pipboy_slider_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff001a00));
    lv_style_set_bg_opa(style, 51);
};

lv_style_t *get_style_crt_pipboy_slider_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_crt_pipboy_slider_MAIN_CHECKED(style);
    }
    return style;
};

void init_style_crt_pipboy_slider_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff00ff00));
    lv_style_set_bg_opa(style, 51);
};

lv_style_t *get_style_crt_pipboy_slider_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_crt_pipboy_slider_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_crt_pipboy_slider_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff00ff00));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_crt_pipboy_slider_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_crt_pipboy_slider_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_crt_pipboy_slider_INDICATOR_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff001a00));
};

lv_style_t *get_style_crt_pipboy_slider_INDICATOR_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_crt_pipboy_slider_INDICATOR_CHECKED(style);
    }
    return style;
};

void init_style_crt_pipboy_slider_KNOB_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff00ff00));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_crt_pipboy_slider_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_crt_pipboy_slider_KNOB_DEFAULT(style);
    }
    return style;
};

void init_style_crt_pipboy_slider_KNOB_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff001a00));
};

lv_style_t *get_style_crt_pipboy_slider_KNOB_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_crt_pipboy_slider_KNOB_CHECKED(style);
    }
    return style;
};

void add_style_crt_pipboy_slider(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_crt_pipboy_slider_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_crt_pipboy_slider_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_crt_pipboy_slider_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_crt_pipboy_slider_INDICATOR_CHECKED(), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_crt_pipboy_slider_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_crt_pipboy_slider_KNOB_CHECKED(), LV_PART_KNOB | LV_STATE_CHECKED);
};

void remove_style_crt_pipboy_slider(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_crt_pipboy_slider_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_crt_pipboy_slider_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_crt_pipboy_slider_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_crt_pipboy_slider_INDICATOR_CHECKED(), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_crt_pipboy_slider_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_crt_pipboy_slider_KNOB_CHECKED(), LV_PART_KNOB | LV_STATE_CHECKED);
};

//
// Style: CRT_Pipboy_Canvas
//

void init_style_crt_pipboy_canvas_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff00ff00));
};

lv_style_t *get_style_crt_pipboy_canvas_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_crt_pipboy_canvas_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_crt_pipboy_canvas(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_crt_pipboy_canvas_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_crt_pipboy_canvas(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_crt_pipboy_canvas_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_crt_pipboy_screen,
        add_style_crt_pipboy_label,
        add_style_crt_pipboy_slider,
        add_style_crt_pipboy_canvas,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_crt_pipboy_screen,
        remove_style_crt_pipboy_label,
        remove_style_crt_pipboy_slider,
        remove_style_crt_pipboy_canvas,
    };
    remove_style_funcs[styleIndex](obj);
}


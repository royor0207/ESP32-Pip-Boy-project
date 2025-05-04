// theme_pipboy.c
#include "theme_pipboy.h"

static lv_style_t style_pipboy;
static lv_obj_t *scanline_img;
static lv_theme_t theme_pipboy;

LV_IMG_DECLARE(crt_scanline_bg_2px_640px); // 640px 高度的掃描線圖片

static void scanline_anim_cb(void *var, int32_t v) {
    lv_obj_set_y((lv_obj_t *)var, v);
}

static void create_scanline_overlay(lv_obj_t *parent) {
    scanline_img = lv_img_create(parent);
    lv_img_set_src(scanline_img, &crt_scanline_bg_2px_640px);
    lv_obj_set_size(scanline_img, 480, 640); // 寬度 480px，高度 640px
    lv_obj_set_style_opa(scanline_img, LV_OPA_30, 0); // 調整透明度
    lv_obj_set_align(scanline_img, LV_ALIGN_TOP_MID);
    lv_obj_set_y(scanline_img, 0);
    lv_obj_add_flag(scanline_img, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_move_foreground(scanline_img);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, scanline_img);
    lv_anim_set_exec_cb(&a, scanline_anim_cb);
    lv_anim_set_values(&a, 0, -320);
    lv_anim_set_time(&a, 3000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}

static const lv_theme_t * get_theme_pipboy(lv_disp_t *disp) {
    LV_UNUSED(disp);
    return &theme_pipboy;
}

static void apply_theme_cb(lv_theme_t *th, lv_obj_t *obj) {
    LV_UNUSED(th);


    const lv_obj_class_t *cls = lv_obj_get_class(obj);

    if (cls == &lv_label_class ||
        cls == &lv_btn_class ||
        cls == &lv_slider_class ||
        cls == &lv_checkbox_class ||
        cls == &lv_list_class ||
        cls == &lv_dropdown_class) {
        lv_obj_add_style(obj, &style_pipboy, 0);
    }
}

void theme_pipboy_init(lv_font_t *font) {
    lv_style_init(&style_pipboy);
    lv_style_set_bg_color(&style_pipboy, lv_color_black());
    lv_style_set_text_color(&style_pipboy, lv_palette_main(LV_PALETTE_LIME));
    lv_style_set_border_color(&style_pipboy, lv_palette_main(LV_PALETTE_LIME));
    lv_style_set_border_width(&style_pipboy, 2);
    lv_style_set_radius(&style_pipboy, 8);
    lv_style_set_pad_all(&style_pipboy, 8);
    lv_style_set_text_font(&style_pipboy, font);

    // 設定自定義 Theme
    theme_pipboy.disp = lv_disp_get_default();
    theme_pipboy.apply_cb = apply_theme_cb;

    lv_disp_set_theme(lv_disp_get_default(), &theme_pipboy);

    // 建立掃描線效果
    create_scanline_overlay(lv_scr_act());
}

void theme_pipboy_refresh(void) {
    create_scanline_overlay(lv_scr_act());
}


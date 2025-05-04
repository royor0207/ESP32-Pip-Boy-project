#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"


#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define CELL_SIZE 12

#define CANVAS_W (BOARD_WIDTH * CELL_SIZE)
#define CANVAS_H (BOARD_HEIGHT * CELL_SIZE)
#define PREVIEW_CANVAS_SIZE 48

// 使用 true color 格式，每像素 2 bytes
static lv_color_t canvas_buf[CANVAS_W * CANVAS_H];
static lv_color_t preview_buf[PREVIEW_CANVAS_SIZE * PREVIEW_CANVAS_SIZE];

void create_screen_main() {
    void *flowState = getFlowState(0, 0);
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    add_style_crt_pipboy_screen(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // label_time
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_time = obj;
            lv_obj_set_pos(obj, 66, 121);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "11 : 45 : 14");
            add_style_crt_pipboy_label(obj);
            lv_obj_set_style_text_font(obj, &ui_font_share_tech_regular_80, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // label_date
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_date = obj;
            lv_obj_set_pos(obj, 66, 224);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "08 / 10 / 1919");
            add_style_crt_pipboy_label(obj);
            lv_obj_set_style_text_font(obj, &ui_font_share_tech_regular_40, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
    void *flowState = getFlowState(0, 0);
}

void create_screen_sensor() {
    void *flowState = getFlowState(0, 1);
    lv_obj_t *obj = lv_obj_create(0);
    objects.sensor = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    add_style_crt_pipboy_screen(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // spo2_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.spo2_label = obj;
            lv_obj_set_pos(obj, 240, 205);
            lv_obj_set_size(obj, 223, 40);
            lv_label_set_text(obj, "SPO");
            add_style_crt_pipboy_label(obj);
            lv_obj_set_style_text_font(obj, &ui_font_share_tech_regular_40, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // bpm_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.bpm_label = obj;
            lv_obj_set_pos(obj, 21, 205);
            lv_obj_set_size(obj, 219, 40);
            lv_label_set_text(obj, "BPM");
            add_style_crt_pipboy_label(obj);
            lv_obj_set_style_text_font(obj, &ui_font_share_tech_regular_40, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // humi_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.humi_label = obj;
            lv_obj_set_pos(obj, 240, 98);
            lv_obj_set_size(obj, 223, 37);
            lv_label_set_text(obj, "HUMI");
            add_style_crt_pipboy_label(obj);
            lv_obj_set_style_text_font(obj, &ui_font_share_tech_regular_40, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // temp_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.temp_label = obj;
            lv_obj_set_pos(obj, 21, 98);
            lv_obj_set_size(obj, 219, 37);
            lv_label_set_text(obj, "TEMP");
            add_style_crt_pipboy_label(obj);
            lv_obj_set_style_text_font(obj, &ui_font_share_tech_regular_40, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_sensor();
}

void tick_screen_sensor() {
    void *flowState = getFlowState(0, 1);
}

void create_screen_music() {
    void *flowState = getFlowState(0, 2);
    lv_obj_t *obj = lv_obj_create(0);
    objects.music = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    add_style_crt_pipboy_screen(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // music1_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.music1_label = obj;
            lv_obj_set_pos(obj, 20, 101);
            lv_obj_set_size(obj, 440, 29);
            lv_label_set_text(obj, "001. Koe naki Sakana (Shin Kawasaki (Kari))");
            lv_obj_add_state(obj, LV_STATE_CHECKED);
            add_style_crt_pipboy_label(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff15171a), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
        }
        {
            // music2_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.music2_label = obj;
            lv_obj_set_pos(obj, 21, 146);
            lv_obj_set_size(obj, 440, 29);
            lv_label_set_text(obj, "002. Void (Nina Iseri, Momoka Kawaragi)");
            add_style_crt_pipboy_label(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff15171a), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
        }
        {
            // music4_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.music4_label = obj;
            lv_obj_set_pos(obj, 21, 239);
            lv_obj_set_size(obj, 440, 29);
            lv_label_set_text(obj, "004. Wrong World");
            add_style_crt_pipboy_label(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff15171a), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
        }
        {
            // music3_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.music3_label = obj;
            lv_obj_set_pos(obj, 21, 192);
            lv_obj_set_size(obj, 440, 29);
            lv_label_set_text(obj, "003. What to raise (Shin Kawasaki)");
            add_style_crt_pipboy_label(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff15171a), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
        }
    }
    
    tick_screen_music();
}

void tick_screen_music() {
    void *flowState = getFlowState(0, 2);
}

void create_screen_game() {
    void *flowState = getFlowState(0, 3);
    lv_obj_t *obj = lv_obj_create(0);
    objects.game = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    add_style_crt_pipboy_screen(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // tetris_score
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.tetris_score = obj;
            lv_obj_set_pos(obj, 21, 58);
            lv_obj_set_size(obj, 124, 22);
            lv_label_set_text(obj, "score : ");
            add_style_crt_pipboy_label(obj);
            lv_obj_set_style_text_font(obj, &ui_font_share_tech_regular_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // start_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.start_label = obj;
            lv_obj_set_pos(obj, 70, 0);
            lv_obj_set_size(obj, 341, 45);
            lv_label_set_text(obj, "Press btn to start");
            add_style_crt_pipboy_label(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_text_opa(obj, 0, LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_letter_space(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff004b00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_share_tech_regular_40, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // tetris_canvas
            lv_obj_t *obj = lv_canvas_create(parent_obj);
            objects.tetris_canvas = obj;
            lv_canvas_set_buffer(obj, canvas_buf, CANVAS_W, CANVAS_H, LV_IMG_CF_TRUE_COLOR);
            lv_obj_set_pos(obj, 180, 80);
            lv_obj_set_size(obj, 120, 240);
            add_style_crt_pipboy_canvas(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // preview_canvas
            lv_obj_t *obj = lv_canvas_create(parent_obj);
            objects.preview_canvas = obj;
            lv_canvas_set_buffer(obj, preview_buf, PREVIEW_CANVAS_SIZE, PREVIEW_CANVAS_SIZE, LV_IMG_CF_TRUE_COLOR);  
            lv_obj_set_pos(obj, 364, 80);
            lv_obj_set_size(obj, 48, 48);
            add_style_crt_pipboy_canvas(obj);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_game();
}

void tick_screen_game() {
    void *flowState = getFlowState(0, 3);
}

void create_screen_setting() {
    void *flowState = getFlowState(0, 4);
    lv_obj_t *obj = lv_obj_create(0);
    objects.setting = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    add_style_crt_pipboy_screen(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // volume_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.volume_label = obj;
            lv_obj_set_pos(obj, 53, 95);
            lv_obj_set_size(obj, 389, 24);
            lv_label_set_text(obj, "  volume");
            lv_obj_add_state(obj, LV_STATE_CHECKED);
            add_style_crt_pipboy_label(obj);
            lv_obj_set_style_text_font(obj, &ui_font_share_tech_regular_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_share_tech_regular_20, LV_PART_MAIN | LV_STATE_CHECKED);
        }
        {
            // volume_slider
            lv_obj_t *obj = lv_slider_create(parent_obj);
            objects.volume_slider = obj;
            lv_obj_set_pos(obj, 151, 104);
            lv_obj_set_size(obj, 282, 5);
            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
            lv_obj_add_state(obj, LV_STATE_CHECKED);
            add_style_crt_pipboy_slider(obj);
        }
    }
    
    tick_screen_setting();
}

void tick_screen_setting() {
    void *flowState = getFlowState(0, 4);
}


extern void add_style(lv_obj_t *obj, int32_t styleIndex);
extern void remove_style(lv_obj_t *obj, int32_t styleIndex);

static const char *screen_names[] = { "Main", "sensor", "music", "game", "setting" };
static const char *object_names[] = { "main", "sensor", "music", "game", "setting", "label_time", "label_date", "spo2_label", "bpm_label", "humi_label", "temp_label", "music1_label", "music2_label", "music4_label", "music3_label", "tetris_score", "start_label", "tetris_canvas", "preview_canvas", "volume_label", "volume_slider" };
static const char *style_names[] = { "CRT_Pipboy_Screen", "CRT_Pipboy_label", "CRT_Pipboy_Slider", "CRT_Pipboy_Canvas" };


typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_sensor,
    tick_screen_music,
    tick_screen_game,
    tick_screen_setting,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    eez_flow_init_styles(add_style, remove_style);
    
    eez_flow_init_screen_names(screen_names, sizeof(screen_names) / sizeof(const char *));
    eez_flow_init_object_names(object_names, sizeof(object_names) / sizeof(const char *));
    eez_flow_init_style_names(style_names, sizeof(style_names) / sizeof(const char *));
    
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
    create_screen_sensor();
    create_screen_music();
    create_screen_game();
    create_screen_setting();
}

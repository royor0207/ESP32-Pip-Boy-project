#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tetris_game.h"
#include "esp_log.h"
#include "lvgl.h"
#include "screens.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "encoder.h"
#include "esp_mac.h"

extern bool lvgl_lock(int timeout_ms);
extern void lvgl_unlock(void);

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define CELL_SIZE 12

#define PREVIEW_BLOCK_SIZE 8
#define CANVAS_W (BOARD_WIDTH * CELL_SIZE)
#define CANVAS_H (BOARD_HEIGHT * CELL_SIZE)
#define PREVIEW_CANVAS_SIZE 48

#define BG_COLOR 0xFF00FF00  // 背景色
#define BLOCK_COLOR 0xFF001A00  // 方塊色
static lv_obj_t *tetris_canvas;
static lv_obj_t *preview_canvas;

// 使用 true color 格式，每像素 2 bytes
static lv_color_t canvas_buf[CANVAS_W * CANVAS_H];
static lv_color_t preview_buf[PREVIEW_CANVAS_SIZE * PREVIEW_CANVAS_SIZE];

typedef struct {
    int x, y;
    int size;
    uint8_t shape[4][4];
} block_t;

// 定義 7 種俄羅斯方塊
static const block_t BLOCKS[] = {
    {0, 0, 3, {{0,1,0}, {1,1,1}, {0,0,0}}}, // T
    {0, 0, 2, {{1,1}, {1,1}}},             // O
    {0, 0, 3, {{1,0,0}, {1,1,1}, {0,0,0}}}, // L
    {0, 0, 3, {{0,0,1}, {1,1,1}, {0,0,0}}}, // J
    {0, 0, 3, {{0,1,1}, {1,1,0}, {0,0,0}}}, // S
    {0, 0, 3, {{1,1,0}, {0,1,1}, {0,0,0}}}, // Z
    {0, 0, 4, {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}}} // I
};
#define BLOCK_COUNT (sizeof(BLOCKS)/sizeof(BLOCKS[0]))

static uint8_t board[BOARD_HEIGHT][BOARD_WIDTH];
static block_t current_block, next_block;
static int current_x, current_y;
static int score = 0;
static bool is_running = false;
static bool is_paused = false;
static bool first_press = true;

static TaskHandle_t tetris_task_handle = NULL;

static void draw_block_on_canvas(int grid_x, int grid_y) {
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_hex(BLOCK_COLOR);
    rect_dsc.bg_opa = LV_OPA_COVER;
    rect_dsc.border_width = 1;  // 或你想要的像素寬度
    rect_dsc.border_color = lv_color_hex(0xFF00FF00);
    rect_dsc.border_opa = LV_OPA_COVER;

    int px = grid_x * CELL_SIZE;
    int py = grid_y * CELL_SIZE;
    lv_canvas_draw_rect(tetris_canvas, px, py, CELL_SIZE, CELL_SIZE, &rect_dsc);
}


static void draw_block_on_preview(int px, int py) {
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_hex(BLOCK_COLOR);
    rect_dsc.bg_opa = LV_OPA_COVER;
    rect_dsc.border_width = 1;
    rect_dsc.border_color = lv_color_hex(0xFF00FF00);
    rect_dsc.border_opa = LV_OPA_COVER;

    lv_canvas_draw_rect(preview_canvas, px, py, PREVIEW_BLOCK_SIZE, PREVIEW_BLOCK_SIZE, &rect_dsc);
}

static void draw_board(void) {
    //保護lvgl操作
    if (!lvgl_lock(-1)) return;


    //防止操控未創建物件
    if (!tetris_canvas || !objects.tetris_score || !lv_obj_get_parent(objects.tetris_score)) {
        ESP_LOGE("TETRIS", "draw_board() skipped: null object or detached!");
        lvgl_unlock();
        return;
    }
    ESP_LOGI("TETRIS", "Draw board start, canvas: %p", tetris_canvas);
    ESP_LOGI("TETRIS", "Canvas size: %d x %d", lv_obj_get_width(tetris_canvas), lv_obj_get_height(tetris_canvas));
    ESP_LOGI("TETRIS", "Canvas pos: %d,%d", lv_obj_get_x(tetris_canvas), lv_obj_get_y(tetris_canvas));


    lv_canvas_fill_bg(tetris_canvas, lv_color_hex(BG_COLOR), LV_OPA_COVER);
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (board[y][x]) {
                draw_block_on_canvas(x, y);
            }
        }
    }
    for (int y = 0; y < current_block.size; y++) {
        for (int x = 0; x < current_block.size; x++) {
            if (current_block.shape[y][x]) {
                draw_block_on_canvas(current_x + x, current_y + y);
            }
        }
    }
    if (objects.tetris_score) {
        char score_text[16] = {0};
        snprintf(score_text, sizeof(score_text), "Score: %d", score);
        lv_label_set_text(objects.tetris_score, score_text);
    }
    lv_obj_invalidate(tetris_canvas);
    lvgl_unlock();
}

static void draw_preview(void) {
    if (!lvgl_lock(-1)) return;
    lv_canvas_fill_bg(preview_canvas, lv_color_hex(BG_COLOR), LV_OPA_COVER);
    const block_t *block = &next_block;
    int block_pixel_width = block->size * PREVIEW_BLOCK_SIZE;
    int block_pixel_height = block->size * PREVIEW_BLOCK_SIZE;
    int offset_x = (PREVIEW_CANVAS_SIZE - block_pixel_width) / 2;
    int offset_y = (PREVIEW_CANVAS_SIZE - block_pixel_height) / 2;

    for (int y = 0; y < block->size; y++) {
        for (int x = 0; x < block->size; x++) {
            if (block->shape[y][x]) {
                int px = offset_x + x * PREVIEW_BLOCK_SIZE;
                int py = offset_y + y * PREVIEW_BLOCK_SIZE;
                draw_block_on_preview(px, py);
            }
        }
    }
    lv_obj_invalidate(preview_canvas);
    lvgl_unlock();
}


// 方塊是否可放置（無碰撞）
static bool can_place(int x, int y, block_t *block) {
    for (int i = 0; i < block->size; i++) {
        for (int j = 0; j < block->size; j++) {
            if (block->shape[i][j]) {
                int nx = x + j;
                int ny = y + i;
                if (nx < 0 || nx >= BOARD_WIDTH || ny < 0 || ny >= BOARD_HEIGHT)
                    return false;
                if (board[ny][nx])
                    return false;
            }
        }
    }
    return true;
}

// 旋轉目前方塊
static void rotate_block(int rotate_val) {
    block_t rotated = current_block;                            // 建立一個副本來進行旋轉
    memset(rotated.shape, 0, sizeof(rotated.shape));            // 先把 shape 陣列清空，避免殘影或錯位
    if (current_block.size == 2) return;                        //方形省去旋轉
    for (int y = 0; y < rotated.size; y++) {
        for (int x = 0; x < rotated.size; x++) {
            if (rotate_val > 0) {
                rotated.shape[y][x] = current_block.shape[rotated.size - x - 1][y];
            } else {
                rotated.shape[y][x] = current_block.shape[x][rotated.size - y - 1];
            }
        }
    }
    if (can_place(current_x, current_y, &rotated)) {
        current_block = rotated;
    } else if (can_place(current_x - 1, current_y, &rotated)) {      // 順時針旋轉：轉換公式為 (x, y) → (y, N - 1 - x)
        current_x--;
        current_block = rotated;
    } else if (can_place(current_x + 1, current_y, &rotated)) {      // 逆時針旋轉：轉換公式為 (x, y) → (N - 1 - y, x)
        current_x++;
        current_block = rotated;
    }
}

static void spawn_block(void) {
    current_block = next_block;
    next_block = BLOCKS[rand() % BLOCK_COUNT];

    current_x = (BOARD_WIDTH - current_block.size) / 2;
    current_y = 0;

    draw_preview();
}

static void lock_block(void) {
    for (int y = 0; y < current_block.size; y++) {
        for (int x = 0; x < current_block.size; x++) {
            if (current_block.shape[y][x]) {
                int bx = current_x + x;
                int by = current_y + y;
                if (bx >= 0 && bx < BOARD_WIDTH && by >= 0 && by < BOARD_HEIGHT) {
                    board[by][bx] = 1;
                }
            }
        }
    }
}

static void clear_lines(void) {
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        bool full = true;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (!board[y][x]) {
                full = false;
                break;
            }
        }
        if (full) {
            for (int row = y; row > 0; row--) {
                for (int x = 0; x < BOARD_WIDTH; x++) {
                    board[row][x] = board[row - 1][x];
                }
            }
            memset(board[0], 0, BOARD_WIDTH);
            score += 100;
            y++;            // 重新檢查該行
        }
    }
}

// 嘗試移動方塊，若失敗會還原
static bool move_block(int dx, int dy) {
    int nx = current_x + dx;
    int ny = current_y + dy;
    if (can_place(nx, ny, &current_block)) {
        current_x = nx;
        current_y = ny;
        return true;
    }
    return false;
}

void tetris_game_init(void) {
    memset(board, 0, sizeof(board));
    score = 0;
    is_running = false;
    is_paused = false;
    first_press = true;

    if (objects.start_label) {
        lv_obj_clear_state(objects.start_label, LV_STATE_CHECKED);
        lv_label_set_text(objects.start_label, "Press Btn to Start");
    }

    next_block = BLOCKS[rand() % BLOCK_COUNT];

    if (objects.tetris_canvas) {
        tetris_canvas = objects.tetris_canvas;
        //lv_canvas_set_buffer(tetris_canvas, canvas_buf, CANVAS_W, CANVAS_H, LV_IMG_CF_TRUE_COLOR);  
        lv_color_t bg = lv_color_make((BG_COLOR >> 16) & 0xFF, (BG_COLOR >> 8) & 0xFF, BG_COLOR & 0xFF);
        lv_canvas_fill_bg(tetris_canvas, bg, LV_OPA_COVER); // 設定背景顏色


        lv_coord_t w = lv_obj_get_width(tetris_canvas);
        lv_coord_t h = lv_obj_get_height(tetris_canvas);
        ESP_LOGI("TETRIS", "Canvas actual size: %d x %d", w, h);
    }

    if (objects.preview_canvas) {
        preview_canvas = objects.preview_canvas;
        //lv_canvas_set_buffer(tetris_canvas, preview_buf, PREVIEW_CANVAS_SIZE, PREVIEW_CANVAS_SIZE, LV_IMG_CF_TRUE_COLOR);       
        lv_color_t bg = lv_color_make((BG_COLOR >> 16) & 0xFF, (BG_COLOR >> 8) & 0xFF, BG_COLOR & 0xFF);
        lv_canvas_fill_bg(preview_canvas, bg, LV_OPA_COVER); // 設定背景顏色

        lv_coord_t w = lv_obj_get_width(preview_canvas);
        lv_coord_t h = lv_obj_get_height(preview_canvas);
        ESP_LOGI("TETRIS", "Canvas actual size: %d x %d", w, h);
    }

    draw_preview();
    draw_board();
}


// 往下掉落，若不能則鎖定
static void drop_block(void) {
    if (!move_block(0, 1)) {
        lock_block();
        clear_lines();
        spawn_block();
        if (!can_place(current_x, current_y, &current_block)) {
            lv_label_set_text(objects.start_label, "Game Over");    
            lv_obj_clear_state(objects.start_label,LV_STATE_CHECKED);
            ESP_LOGI("k","game over");
            is_running = false;
            is_paused = false;
            memset(&current_block, 0, sizeof(current_block));
            vTaskDelay(pdMS_TO_TICKS(5000));
            tetris_game_init();
        }
    }
}

static void toggle_pause(void) {
    is_paused = !is_paused;
    if (is_paused) {
        lv_obj_add_state(objects.start_label, LV_STATE_CHECKED);
    } else {
        lv_label_set_text(objects.start_label, "Pause");
        lv_obj_clear_state(objects.start_label, LV_STATE_CHECKED);
    }
}

static void tetris_game_task(void *arg) {
    vTaskDelay(pdMS_TO_TICKS(200));
    tetris_game_init();
    if (!objects.tetris_canvas || !objects.preview_canvas) {
        ESP_LOGE("TETRIS", "UI canvas not ready!");
        vTaskDelete(NULL);
        return;
    }

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(200));

        if (!is_running) {
            if (is_encoder_button_pressed(0)) {
                is_running = true;
                is_paused = false;

                read_and_clear_encoder_pcnt(0);  // encoder 0: 旋轉
                read_and_clear_encoder_pcnt(1);  // encoder 1: 左右移動

                spawn_block();
                draw_board();               
                vTaskDelay(pdMS_TO_TICKS(300));
            }
            continue;
        }
        
        //遊戲啟動狀態
        if(!first_press){
            if (is_encoder_button_pressed(0)) {
                toggle_pause();
                vTaskDelay(pdMS_TO_TICKS(100));
            }
        }
        first_press = false;



        if (!is_paused) continue;
        //沒暫停繼續

        //檢測旋轉項
        int rotate_val = read_and_clear_encoder_pcnt(0);
        if (rotate_val != 0) {
            rotate_block(rotate_val);
            draw_board();
        }

        //檢測移動項
        int move_dir = read_and_clear_encoder_pcnt(1);
        if (move_dir != 0) {
            move_block(-move_dir, 0);
            draw_board();
        }
        draw_board();
        drop_block();

    }
}

void tetris_game_start(void) {
    if (tetris_task_handle == NULL) {
        vTaskDelay(pdMS_TO_TICKS(200));
        xTaskCreate(tetris_game_task, "tetris_game", 4096, NULL, 5, &tetris_task_handle);
        ESP_LOGI("tetris_game", "tetris game start");
    }
}

void tetris_game_stop(void) {
    if (tetris_task_handle != NULL) {
        vTaskDelete(tetris_task_handle);
        tetris_task_handle = NULL;
        ESP_LOGI("tetris_game", "tetris game stop");
    }
}

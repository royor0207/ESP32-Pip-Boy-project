#include <stdio.h>
#include "time.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_spiffs.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "lvgl.h"
#include "hardware_init.h"
#include "screen_switcher.h"
#ifdef __cplusplus
}
#endif


#include "ui.h"
#include "wifi_connect.h"
#include "time_display.h"

static const char *TAG = "example";

#define EXAMPLE_LCD_PIXEL_CLOCK_HZ     CONFIG_EXAMPLE_LCD_PIXEL_CLOCK_HZ

#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL  1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
#define EXAMPLE_PIN_NUM_DATA0          19
#define EXAMPLE_PIN_NUM_DATA1          23
#define EXAMPLE_PIN_NUM_DATA2          4
#define EXAMPLE_PIN_NUM_DATA3          16
#define EXAMPLE_PIN_NUM_DATA4          14
#define EXAMPLE_PIN_NUM_DATA5          17
#define EXAMPLE_PIN_NUM_DATA6          33
#define EXAMPLE_PIN_NUM_DATA7          18
#define EXAMPLE_PIN_NUM_PCLK           26
#define EXAMPLE_PIN_NUM_CS             -1           //單顯示屏可直接拉低
#define EXAMPLE_PIN_NUM_DC             27
#define EXAMPLE_PIN_NUM_RST            13
#define EXAMPLE_PIN_NUM_BK_LIGHT       -1

#define EXAMPLE_LCD_H_RES              480
#define EXAMPLE_LCD_V_RES              320

// Bit number used to represent command and parameter
#define EXAMPLE_LCD_CMD_BITS           8
#define EXAMPLE_LCD_PARAM_BITS         8

#define EXAMPLE_LVGL_TICK_PERIOD_MS    2
#define EXAMPLE_LVGL_TASK_MAX_DELAY_MS 500
#define EXAMPLE_LVGL_TASK_MIN_DELAY_MS 10
#define EXAMPLE_LVGL_TASK_STACK_SIZE   (4 * 1024)
#define EXAMPLE_LVGL_TASK_PRIORITY     2
#define NUM 20

#define EXAMPLE_DMA_BURST_SIZE         64 // 16, 32, 64. Higher burst size can improve the performance when the DMA buffer comes from PSRAM


static SemaphoreHandle_t lvgl_mux = NULL;

#include "esp_heap_caps.h"

//查看剩餘記憶體
void print_free_heap_info() {
    ESP_LOGI("MEM", "Free heap: %d", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    ESP_LOGI("MEM", "Largest free block: %d", heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));
}


// 初始化 LVGL 與螢幕
static bool example_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    lv_disp_drv_t *disp_driver = (lv_disp_drv_t *)user_ctx;
    lv_disp_flush_ready(disp_driver);
    return false;
}

static void example_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    // copy a buffer's content to a specific area of the display
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}

static void example_increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

extern "C"{
    bool lvgl_lock(int timeout_ms)
    {
        // Convert timeout in milliseconds to FreeRTOS ticks
        // If `timeout_ms` is set to -1, the program will block until the condition is met
        const TickType_t timeout_ticks = (timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
        return xSemaphoreTakeRecursive(lvgl_mux, timeout_ticks) == pdTRUE;
    }

    void lvgl_unlock(void)
    {
        xSemaphoreGiveRecursive(lvgl_mux);
    }
}
static void lvgl_port_task(void *arg)
{
    ESP_LOGI("LVGL", "Starting LVGL task");
    uint32_t task_delay_ms = 500;
    while (1) {
        if (lvgl_lock(-1)) {
            task_delay_ms = lv_timer_handler();     //執行 LVGL 計時器（動畫、轉場、按鍵長按等）執行畫面重繪排程（如果有畫面變更） 根據需要回傳下次呼叫的建議延遲時間         
            ui_tick();
            lvgl_unlock();
        }
        //task_delay_ms = screen_task_get_interval();  // 根據畫面取得延遲(scanline動畫更新停用)
        if (task_delay_ms > EXAMPLE_LVGL_TASK_MAX_DELAY_MS) {
            task_delay_ms = EXAMPLE_LVGL_TASK_MAX_DELAY_MS;
        }
        if (task_delay_ms < EXAMPLE_LVGL_TASK_MIN_DELAY_MS) {
            task_delay_ms = EXAMPLE_LVGL_TASK_MIN_DELAY_MS;
        }

        vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
    }
}

void example_init_i80_bus(esp_lcd_panel_io_handle_t *io_handle, void *user_ctx)
{
    ESP_LOGI(TAG, "Initialize Intel 8080 bus");
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = EXAMPLE_PIN_NUM_DC,
        .wr_gpio_num = EXAMPLE_PIN_NUM_PCLK,
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .data_gpio_nums = {
            EXAMPLE_PIN_NUM_DATA0,
            EXAMPLE_PIN_NUM_DATA1,
            EXAMPLE_PIN_NUM_DATA2,
            EXAMPLE_PIN_NUM_DATA3,
            EXAMPLE_PIN_NUM_DATA4,
            EXAMPLE_PIN_NUM_DATA5,
            EXAMPLE_PIN_NUM_DATA6,
            EXAMPLE_PIN_NUM_DATA7,
        },
        .bus_width = CONFIG_EXAMPLE_LCD_I80_BUS_WIDTH,
        .max_transfer_bytes = EXAMPLE_LCD_H_RES * NUM * sizeof(uint16_t),
        .dma_burst_size = EXAMPLE_DMA_BURST_SIZE,
    };
    ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = EXAMPLE_PIN_NUM_CS,
        .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .trans_queue_depth = 10,
        .on_color_trans_done = example_notify_lvgl_flush_ready,
        .user_ctx = user_ctx,
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
        .lcd_param_bits = EXAMPLE_LCD_PARAM_BITS,       
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .flags = {
            .swap_color_bytes = !LV_COLOR_16_SWAP, // Swap can be done in LvGL (default) or DMA
        },
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, io_handle));
}

void example_init_lcd_panel(esp_lcd_panel_io_handle_t io_handle, esp_lcd_panel_handle_t *panel)
{
    esp_lcd_panel_handle_t panel_handle = NULL;
    ESP_LOGI(TAG, "Install LCD driver of ili9486");
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    // Set inversion, x/y coordinate order, x/y mirror according to your LCD module spec
    // the gap is LCD panel specific, even panels with the same driver IC, can have different gap value
    esp_lcd_panel_swap_xy(panel_handle, true);
    esp_lcd_panel_invert_color(panel_handle, false);
    // ILI9341 is very similar to ST7789 and shares the same driver.
    // Anything unconventional (such as this custom gamma table) can
    // be issued here in user code and need not modify the driver.
    esp_lcd_panel_io_tx_param(io_handle, 0xF2, (uint8_t[]) {
        0
    }, 1); // 3Gamma function disable
    esp_lcd_panel_io_tx_param(io_handle, 0x26, (uint8_t[]) {
        1
    }, 1); // Gamma curve 1 selected
    esp_lcd_panel_io_tx_param(io_handle, 0xE0, (uint8_t[]) {          // Set positive gamma
        0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00
    }, 15);
    esp_lcd_panel_io_tx_param(io_handle, 0xE1, (uint8_t[]) {          // Set negative gamma
        0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F
    }, 15);

    *panel = panel_handle;
}


extern "C"  void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_DEBUG);

    static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t disp_drv;      // contains callback functions

    esp_lcd_panel_io_handle_t io_handle = NULL;
    example_init_i80_bus(&io_handle, &disp_drv);

    esp_lcd_panel_handle_t panel_handle = NULL;
    example_init_lcd_panel(io_handle, &panel_handle);

    // Stub: user can flush pre-defined pattern to the screen before we turn on the screen or backlight

    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();

    // alloc draw buffers used by LVGL
    // it's recommended to choose the size of the draw buffer(s) to be at least 1/10 screen sized
    uint32_t draw_buf_alloc_caps = 0;

    lv_color_t *buf1 = (lv_color_t *)esp_lcd_i80_alloc_draw_buffer(io_handle, EXAMPLE_LCD_H_RES * NUM * sizeof(lv_color_t), draw_buf_alloc_caps);
    lv_color_t *buf2 = (lv_color_t *)esp_lcd_i80_alloc_draw_buffer(io_handle, EXAMPLE_LCD_H_RES * NUM * sizeof(lv_color_t), draw_buf_alloc_caps);
    assert(buf1);
    assert(buf2);
    ESP_LOGI(TAG, "buf1@%p, buf2@%p", buf1, buf2);
    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, EXAMPLE_LCD_H_RES * NUM);

    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;
    disp_drv.flush_cb = example_lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel_handle;
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

    ESP_LOGI(TAG, "Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &example_increase_lvgl_tick,
        .name = "lvgl_tick"
    };

    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));

    lvgl_mux = xSemaphoreCreateRecursiveMutex();
    assert(lvgl_mux);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
    
    screen_queue = xQueueCreate(10, sizeof(screen_id_t));    
    wifi_init_sta("TP-Link_IoT_CD44", "49095652z");

    hardware_init(); 

    screen_switcher_start_task();

    time_display_init();

    if (lvgl_lock(-1)) {
        ui_init();
        ESP_LOGI("UI_update","UI_INIT");
        lvgl_unlock();
    }
    time_display_start();
    
    ESP_LOGI(TAG, "Create LVGL task");
    xTaskCreate(lvgl_port_task, "LVGL", EXAMPLE_LVGL_TASK_STACK_SIZE, NULL, EXAMPLE_LVGL_TASK_PRIORITY, NULL);      //lvgl更新任務

    music_player_set_volume_percent(25);


    print_free_heap_info();
}

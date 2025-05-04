#include "time_display.h"
#include "lvgl.h"
#include "esp_log.h"
#include "screens.h"
#include <stdio.h>
#include <esp_sntp.h>
#include <esp_log.h>
#include <time.h>
#include <sys/time.h>


static const char *TAG = "time_display";
static lv_timer_t* time_timer = NULL;

static void update_time_cb(lv_timer_t* timer) {
    time_t now;
    struct tm timeinfo;
    char time_str[16];
    char date_str[32];

    time(&now);
    localtime_r(&now, &timeinfo);

    snprintf(time_str, sizeof(time_str), "%02d : %02d : %02d",
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    snprintf(date_str, sizeof(date_str), "%02d / %02d / %04d",
             timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_year + 1900);

    lv_label_set_text(objects.label_time, time_str);
    lv_label_set_text(objects.label_date, date_str);
}

static void obtain_time_once(void) {    
    ESP_LOGI("SNTP", "Initializing SNTP");


    setenv("TZ", "CST-8", 1);
    tzset();

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    // 等待時間同步完成
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 10;
    while (timeinfo.tm_year < (2020 - 1900) && ++retry < retry_count) {
        ESP_LOGI("SNTP", "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(pdMS_TO_TICKS(2000));
        time(&now);
        localtime_r(&now, &timeinfo);
    }
    ESP_LOGI("SNTP", "TIME : %d/%d", timeinfo.tm_year, timeinfo.tm_yday);
}

void time_display_init(void) {
    obtain_time_once();  // 初次同步實際時間
    time_timer = lv_timer_create(update_time_cb, 1000, NULL); // 每秒更新
    lv_timer_pause(time_timer); // 預設暫停，首頁才啟動
    ESP_LOGI("time_display","start");

}

void time_display_start(void) {
    if (time_timer) {
        lv_timer_resume(time_timer);
        ESP_LOGI("time_display","resume");

    }
}

void time_display_stop(void) {
    if (time_timer) {
        lv_timer_pause(time_timer);
        ESP_LOGI("time_display","stop");
    }
}
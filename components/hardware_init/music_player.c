#include <string.h>
#include "music_player.h"
#include "driver/uart.h"
#include "esp_log.h"

#define UART_NUM UART_NUM_1
#define BUF_SIZE 128
#define UART_PIN_RX 32
#define UART_PIN_TX 39

static const char *TAG = "MusicPlayer";

//extern int32_t new_value;


// 初始化 UART（無變動）
esp_err_t music_player_init(void) {
    uart_config_t uart_config = {
        .baud_rate = 9600,  
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    esp_err_t err;
    if ((err = uart_driver_install(UART_NUM, BUF_SIZE * 4, 0, 0, NULL, 0)) != ESP_OK) return err;
    if ((err = uart_param_config(UART_NUM, &uart_config)) != ESP_OK) return err;
    if ((err = uart_set_pin(UART_NUM, UART_PIN_TX, UART_PIN_RX , UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE)) != ESP_OK) return err;

    ESP_LOGI(TAG, "UART for music player initialized");
    return ESP_OK;
}

// DFPlayer 指令封裝
static void send_dfplayer_command(uint8_t cmd, uint16_t param) {
    uint8_t buf[10];
    buf[0] = 0x7E;
    buf[1] = 0xFF;
    buf[2] = 0x06;
    buf[3] = cmd;
    buf[4] = 0x01; // request feedback
    buf[5] = (param >> 8) & 0xFF;
    buf[6] = param & 0xFF;

    uint16_t checksum = 0 - (buf[1] + buf[2] + buf[3] + buf[4] + buf[5] + buf[6]);
    buf[7] = (checksum >> 8) & 0xFF;
    buf[8] = checksum & 0xFF;
    buf[9] = 0xEF;

    uart_write_bytes(UART_NUM, (const char*)buf, 10);
    ESP_LOGI(TAG, "DFPlayer CMD Sent: 0x%02X, Param: %d", cmd, param);
}

static void print_dfplayer_response(void) {
    uint8_t response[10];
    int length = uart_read_bytes(UART_NUM, response, sizeof(response), pdMS_TO_TICKS(200));

    if (length > 0) {
        printf("DFPlayer response (%d bytes): ", length);
        for (int i = 0; i < length; i++) {
            printf("0x%02X ", response[i]);
        }
        printf("\n");
    } else {
        printf("No response received from DFPlayer.\n");
    }
}


// 播放第 N 首
esp_err_t music_player_play(int index) {
    ESP_LOGI("music","playing %d", index);
    send_dfplayer_command(0x03, index);
    //music_player_set_volume_percent(new_value);
    print_dfplayer_response();
    return ESP_OK;
}

// 暫停
esp_err_t music_player_pause(void) {
    send_dfplayer_command(0x0E, 0);
    print_dfplayer_response();
    return ESP_OK;
}

// 下一首
esp_err_t music_player_next(void) {
    send_dfplayer_command(0x01, 0);
    return ESP_OK;
}

// 上一首
esp_err_t music_player_prev(void) {
    send_dfplayer_command(0x02, 0);
    return ESP_OK;
}

//暫停後繼續撥放
esp_err_t music_player_resume(void) {
    send_dfplayer_command(0x0D, 0);
    return ESP_OK;
}

// 設定音量（0~30）
esp_err_t music_player_set_volume_percent(int percent) {
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    // 將百分比轉成 DFPlayer 的 0~30 音量範圍
    int volume = (percent * 30) / 100;
    send_dfplayer_command(0x06, volume);
    print_dfplayer_response();
    ESP_LOGI(TAG, "volume: %d", volume);
    return ESP_OK;
}

// 讀取狀態功能暫不實作（保留原本的 stub）
esp_err_t music_player_get_status(char *buffer, size_t len) {
    int read = uart_read_bytes(UART_NUM, (uint8_t *)buffer, len - 1, pdMS_TO_TICKS(100));
    if (read > 0) {
        buffer[read] = '\0';
        return ESP_OK;
    }
    return ESP_FAIL;
}
#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include "esp_err.h"

// 初始化 UART 並設定 minimp3 模組
esp_err_t music_player_init(void);

// 傳送播放指令
esp_err_t music_player_play(int index);
esp_err_t music_player_pause(void);
esp_err_t music_player_next(void);
esp_err_t music_player_prev(void);
esp_err_t music_player_resume(void);
esp_err_t music_player_set_volume_percent(int percent);

// 讀取當前播放資訊（例如檔名或狀態）
esp_err_t music_player_get_status(char *buffer, size_t len);



#endif

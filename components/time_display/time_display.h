#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void time_display_init(void);   // SNTP 初始化 + Timer 創建與暫停
void time_display_start(void);  // 切換到首頁時呼叫以啟用時間更新
void time_display_stop(void);   // 離開首頁時暫停時間更新

#ifdef __cplusplus
}
#endif
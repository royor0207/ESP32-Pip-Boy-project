#ifdef __cplusplus
extern "C" {
#endif


void setting_task_start(void);   // SNTP 初始化 + Timer 創建與暫停
void setting_task_stop(void);  // 切換到首頁時呼叫以啟用時間更新

static int32_t new_value;

#ifdef __cplusplus
}
#endif
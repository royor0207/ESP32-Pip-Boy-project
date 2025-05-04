#ifndef MAX30102_H
#define MAX30102_H

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化 MAX30102 感測器
 * 
 * @return esp_err_t 
 */
esp_err_t max30102_init(void);

/**
 * @brief 讀取並分析 MAX30102 數據
 * 
 * @param[out] heart_rate 估算後的心率（bpm）
 * @param[out] spo2       估算後的血氧飽和度（%）
 * @return esp_err_t 
 */
esp_err_t max30102_read(uint8_t *heart_rate, uint8_t *spo2);

#ifdef __cplusplus
}
#endif

#endif // MAX30102_H

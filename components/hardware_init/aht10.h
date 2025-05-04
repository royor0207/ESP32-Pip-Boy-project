#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t aht10_init(void);
esp_err_t aht10_read_data(float *temperature, float *humidity);

#ifdef __cplusplus
}
#endif

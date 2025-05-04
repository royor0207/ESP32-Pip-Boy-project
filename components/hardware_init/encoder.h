#ifndef ENCODER_H
#define ENCODER_H

#include "driver/gpio.h"
#include "driver/pcnt.h"

#ifdef __cplusplus
extern "C" {
#endif

// 初始化所有旋轉編碼器
void encoder_init(void);

// 讀取指定編碼器的增量值（自動清除）
int read_and_clear_encoder_pcnt(int encoder_num);  // 0 or 1

// 讀取編碼器按鈕狀態
int is_encoder_button_pressed(int encoder_num); // 0 or 1

#ifdef __cplusplus
}
#endif

#endif // ENCODER_H
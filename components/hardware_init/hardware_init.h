#ifndef HARDWARE_INIT_H
#define HARDWARE_INIT_H

#include "driver/gpio.h"
#include "encoder.h"
#include "aht10.h"
#include "max30102.h"
#include "music_player.h"
#include "esp_log.h"


#define GPIO_SCREEN_0 GPIO_NUM_15
#define GPIO_SCREEN_1 GPIO_NUM_2
#define GPIO_SCREEN_2 GPIO_NUM_0
#define GPIO_SCREEN_3 GPIO_NUM_25
#define GPIO_SCREEN_4 GPIO_NUM_3

void hardware_init(void);

#endif // HARDWARE_INIT_H

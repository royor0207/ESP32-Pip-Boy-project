#ifndef MUSIC_SCREEN_H
#define MUSIC_SCREEN_H

#include <stdbool.h>

void music_screen_start(void);
void music_screen_stop(void);

// 外部呼叫：設定音量 (0–100)
void music_screen_set_volume(int volume);

#endif // MUSIC_SCREEN_H

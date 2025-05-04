#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#include <string.h>


#define WIFI_MAX_RETRY 5

#ifdef __cplusplus
extern "C" {
#endif

// 初始化 Wi-Fi
void wifi_init_sta(const char *ssid, const char *password);

#ifdef __cplusplus
}
#endif

#endif // WIFI_CONNECT_H

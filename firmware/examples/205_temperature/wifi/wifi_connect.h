/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Application core main function for standard \n
 *
 * History: \n
 * 2022-07-27, Create file. \n
 */

#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#include <stdint.h>

int wifi_connect(const char *ssid, const char *psk);

int func_wifi_get_ip_address
    (
    char *ip_buf, 
    unsigned int buf_size
    );

int func_wifi_connect
    (
    const char *ssid,
    const char *psk
    );

#endif
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Application core main function for standard \n
 *
 * History: \n
 * 2022-07-27, Create file. \n
 */

#include "lwip/netifapi.h"
#include "wifi_hotspot.h"
#include "wifi_hotspot_config.h"
#include "td_base.h"
#include "td_type.h"
#include "stdlib.h"
// #include "uart.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "soc_osal.h"
#include "lwip/nettool/misc.h"
#include "wifi_connect.h"
#include "std_def.h"

#define WIFI_IFNAME_MAX_SIZE 16
#define WIFI_MAX_SSID_LEN 33
#define WIFI_SCAN_AP_LIMIT 64
#define WIFI_MAC_LEN 6
#define WIFI_STA_SAMPLE_LOG "[WIFI_STA_SAMPLE]"
#define WIFI_NOT_AVALLIABLE 0
#define WIFI_AVALIABE 1
#define WIFI_GET_IP_MAX_COUNT 300

#define WIFI_TASK_PRIO (osPriority_t)(13)
#define WIFI_TASK_DURATION_MS 2000
#define WIFI_TASK_STACK_SIZE 0x1000

static td_void wifi_scan_state_changed(td_s32 state, td_s32 size);
static td_void wifi_connection_changed(td_s32 state, const wifi_linked_info_stru *info, td_s32 reason_code);

wifi_event_stru wifi_event_cb = {
    .wifi_event_connection_changed = wifi_connection_changed,
    .wifi_event_scan_state_changed = wifi_scan_state_changed,
};

enum {
    WIFI_STA_SAMPLE_INIT = 0,     /* 0:初始态 */
    WIFI_STA_SAMPLE_SCANING,      /* 1:扫描中 */
    WIFI_STA_SAMPLE_SCAN_DONE,    /* 2:扫描完成 */
    WIFI_STA_SAMPLE_FOUND_TARGET, /* 3:匹配到目标AP */
    WIFI_STA_SAMPLE_CONNECTING,   /* 4:连接中 */
    WIFI_STA_SAMPLE_CONNECT_DONE, /* 5:关联成功 */
    WIFI_STA_SAMPLE_GET_IP,       /* 6:获取IP */
} g_wifi_state_enum;

static td_u8 g_wifi_state = WIFI_STA_SAMPLE_INIT;

/*****************************************************************************
  STA 扫描事件回调函数
*****************************************************************************/
static td_void wifi_scan_state_changed(td_s32 state, td_s32 size)
{
    UNUSED(state);
    UNUSED(size);
    printf("%s::Scan done!.\r\n", WIFI_STA_SAMPLE_LOG);
    g_wifi_state = WIFI_STA_SAMPLE_SCAN_DONE;
    return;
}

/*****************************************************************************
  STA 关联事件回调函数
*****************************************************************************/
static td_void wifi_connection_changed(td_s32 state, const wifi_linked_info_stru *info, td_s32 reason_code)
{
    UNUSED(info);
    UNUSED(reason_code);

    if (state == WIFI_NOT_AVALLIABLE) {
        printf("%s::Connect fail!. try agin !\r\n", WIFI_STA_SAMPLE_LOG);
        g_wifi_state = WIFI_STA_SAMPLE_INIT;
    } else {
        printf("%s::Connect succ!.\r\n", WIFI_STA_SAMPLE_LOG);
        g_wifi_state = WIFI_STA_SAMPLE_CONNECT_DONE;
    }
}

/*****************************************************************************
  STA 匹配目标AP
*****************************************************************************/
td_s32 example_get_match_network(wifi_sta_config_stru *expected_bss, const char *ssid, const char *psk)
{
    td_s32 ret;
    td_u32 num = 64; /* 64:扫描到的Wi-Fi网络数量 */
    // td_char ssid[] = "test";
    // td_char key[] = "YOUR_WIFI_PASSWORD"; /* 待连接的网络接入密码 */
    td_bool find_ap = TD_FALSE;
    td_u8 bss_index;
    /* 获取扫描结果 */
    td_u32 scan_len = sizeof(wifi_scan_info_stru) * WIFI_SCAN_AP_LIMIT;
    wifi_scan_info_stru *result = osal_kmalloc(scan_len, OSAL_GFP_ATOMIC);
    if (result == TD_NULL) {
        return -1;
    }
    memset_s(result, scan_len, 0, scan_len);
    ret = wifi_sta_get_scan_info(result, &num);
    if (ret != 0) {
        osal_kfree(result);
        return -1;
    }
    /* 筛选扫描到的Wi-Fi网络，选择待连接的网络 */
    for (bss_index = 0; bss_index < num; bss_index++) {
        if (strlen(ssid) == strlen(result[bss_index].ssid)) {
            if (memcmp(ssid, result[bss_index].ssid, strlen(ssid)) == 0) {
                find_ap = TD_TRUE;
                break;
            }
        }
    }
    /* 未找到待连接AP,可以继续尝试扫描或者退出 */
    if (find_ap == TD_FALSE) {
        osal_kfree(result);
        return -1;
    }
    /* 找到网络后复制网络信息和接入密码 */
    if (memcpy_s(expected_bss->ssid, WIFI_MAX_SSID_LEN, ssid, strlen(ssid)) != 0) {
        osal_kfree(result);
        return -1;
    }
    if (memcpy_s(expected_bss->bssid, WIFI_MAC_LEN, result[bss_index].bssid, WIFI_MAC_LEN) != 0) {
        osal_kfree(result);
        return -1;
    }
    expected_bss->security_type = result[bss_index].security_type;
    if (memcpy_s(expected_bss->pre_shared_key, WIFI_MAX_SSID_LEN, psk, strlen(psk)) != 0) {
        osal_kfree(result);
        return -1;
    }
    expected_bss->ip_type = 1; /* 1：IP类型为动态DHCP获取 */
    osal_kfree(result);
    return 0;
}

/*****************************************************************************
  STA 关联状态查询
*****************************************************************************/
td_bool example_check_connect_status(td_void)
{
    td_u8 index;
    wifi_linked_info_stru wifi_status;
    /* 获取网络连接状态，共查询5次，每次间隔500ms */
    for (index = 0; index < 5; index++) {
        (void)osDelay(50); /* 50: 延时500ms */
        memset_s(&wifi_status, sizeof(wifi_linked_info_stru), 0, sizeof(wifi_linked_info_stru));
        if (wifi_sta_get_ap_info(&wifi_status) != 0) {
            continue;
        }
        if (wifi_status.conn_state == 1) {
            return 0; /* 连接成功退出循环 */
        }
    }
    return -1;
}

/*****************************************************************************
  STA DHCP状态查询
*****************************************************************************/
td_bool example_check_dhcp_status(struct netif *netif_p, td_u32 *wait_count)
{
    if ((ip_addr_isany(&(netif_p->ip_addr)) == 0) && (*wait_count <= WIFI_GET_IP_MAX_COUNT)) {
        /* DHCP成功 */
        printf("%s::STA DHCP success.\r\n", WIFI_STA_SAMPLE_LOG);
        return 0;
    }

    if (*wait_count > WIFI_GET_IP_MAX_COUNT) {
        printf("%s::STA DHCP timeout, try again !.\r\n", WIFI_STA_SAMPLE_LOG);
        *wait_count = 0;
        g_wifi_state = WIFI_STA_SAMPLE_INIT;
    }
    return -1;
}

td_s32 example_sta_function(const char *ssid, const char *psk)
{
    td_char ifname[WIFI_IFNAME_MAX_SIZE + 1] = "wlan0"; /* 创建的STA接口名 */
    wifi_sta_config_stru expected_bss = {0};            /* 连接请求信息 */
    struct netif *netif_p = TD_NULL;
    td_u32 wait_count = 0;

    /* 创建STA接口 */
    if (wifi_sta_enable() != 0) {
        return -1;
    }
    printf("%s::STA enable succ.\r\n", WIFI_STA_SAMPLE_LOG);

    do {
        (void)osDelay(1); /* 1: 等待10ms后判断状态 */
        if (g_wifi_state == WIFI_STA_SAMPLE_INIT) {
            printf("%s::Scan start!\r\n", WIFI_STA_SAMPLE_LOG);
            g_wifi_state = WIFI_STA_SAMPLE_SCANING;
            /* 启动STA扫描 */
            if (wifi_sta_scan() != 0) {
                g_wifi_state = WIFI_STA_SAMPLE_INIT;
                continue;
            }
        } else if (g_wifi_state == WIFI_STA_SAMPLE_SCAN_DONE) {
            /* 获取待连接的网络 */
            if (example_get_match_network(&expected_bss, ssid, psk) != 0) {
                printf("%s::Do not find AP, try again !\r\n", WIFI_STA_SAMPLE_LOG);
                g_wifi_state = WIFI_STA_SAMPLE_INIT;
                continue;
            }
            g_wifi_state = WIFI_STA_SAMPLE_FOUND_TARGET;
        } else if (g_wifi_state == WIFI_STA_SAMPLE_FOUND_TARGET) {
            printf("%s::Connect start.\r\n", WIFI_STA_SAMPLE_LOG);
            g_wifi_state = WIFI_STA_SAMPLE_CONNECTING;
            /* 启动连接 */
            if (wifi_sta_connect(&expected_bss) != 0) {
                g_wifi_state = WIFI_STA_SAMPLE_INIT;
                continue;
            }
        } else if (g_wifi_state == WIFI_STA_SAMPLE_CONNECT_DONE) {
            printf("%s::DHCP start.\r\n", WIFI_STA_SAMPLE_LOG);
            g_wifi_state = WIFI_STA_SAMPLE_GET_IP;
            netif_p = netifapi_netif_find(ifname);
            if (netif_p == TD_NULL || netifapi_dhcp_start(netif_p) != 0) {
                printf("%s::find netif or start DHCP fail, try again !\r\n", WIFI_STA_SAMPLE_LOG);
                g_wifi_state = WIFI_STA_SAMPLE_INIT;
                continue;
            }
        } else if (g_wifi_state == WIFI_STA_SAMPLE_GET_IP) {
            if (example_check_dhcp_status(netif_p, &wait_count) == 0) {
                netifapi_netif_common(netif_p, dhcp_clients_info_show, NULL);
                break;
            }
            wait_count++;
        }
    } while (1);

    return 0;
}

int wifi_connect(const char *ssid, const char *psk)
{
    /* 注册事件回调 */
    if (wifi_register_event_cb(&wifi_event_cb) != 0) {
        printf("%s::wifi_event_cb register fail.\r\n", WIFI_STA_SAMPLE_LOG);
        return -1;
    }
    printf("%s::wifi_event_cb register succ.\r\n", WIFI_STA_SAMPLE_LOG);

    /* 等待wifi初始化完成 */
    while (wifi_is_wifi_inited() == 0) {
        (void)osDelay(10); /* 1: 等待100ms后判断状态 */
    }
    printf("%s::wifi init succ.\r\n", WIFI_STA_SAMPLE_LOG);

    if (example_sta_function(ssid, psk) != 0) {
        printf("%s::example_sta_function fail.\r\n", WIFI_STA_SAMPLE_LOG);
        return -1;
    }
    return 0;
}

/*****************************************************************************
  获取当前连接的IP地址
*****************************************************************************/
int func_wifi_get_ip_address
    (
    char *ip_buf, 
    unsigned int buf_size
    )
{
    struct netif *netif_p = TD_NULL;
    char ifname[WIFI_IFNAME_MAX_SIZE + 1] = "wlan0";
    
    netif_p = netifapi_netif_find(ifname);
    if (netif_p == TD_NULL) {
        printf("%s::Failed to find netif!\r\n", WIFI_STA_SAMPLE_LOG);
        return -1;
    }
    
    // 正确检查IP地址是否未分配
    if (ip_addr_isany(&netif_p->ip_addr)) {
        printf("%s::IP address not assigned yet!\r\n", WIFI_STA_SAMPLE_LOG);
        return -1;
    }
    
    // 使用lwip的ip4addr_ntoa函数转换IPv4地址
    const char *ip_str = ip4addr_ntoa(&netif_p->ip_addr.u_addr.ip4);
    if (ip_str != NULL) {
        snprintf(ip_buf, buf_size, "%s", ip_str);
        return 0;
    }
    
    return -1;
}


td_s32 example_sta_function_with_retry(const char *ssid, const char *psk)
{
    td_char ifname[WIFI_IFNAME_MAX_SIZE + 1] = "wlan0"; /* 创建的STA接口名 */
    wifi_sta_config_stru expected_bss = {0};            /* 连接请求信息 */
    struct netif *netif_p = TD_NULL;
    td_u32 wait_count = 0;
    td_u32 total_attempts = 0;                          /* 总尝试次数计数器 */
    const td_u32 max_attempts = 10;                     /* 最大尝试次数 */

    /* 创建STA接口 */
    if (wifi_sta_enable() != 0) {
        printf("%s::STA enable failed.\r\n", WIFI_STA_SAMPLE_LOG);
        return -1;
    }
    printf("%s::STA enable succ.\r\n", WIFI_STA_SAMPLE_LOG);

    do {
        /* 检查是否超过最大尝试次数 */
        if (total_attempts >= max_attempts) {
            printf("%s::Reached max attempts (%d), connect failed.\r\n", 
                  WIFI_STA_SAMPLE_LOG, max_attempts);
            /* 清理资源：关闭STA接口 */
            wifi_sta_disable();
            return -2; /* 超过最大尝试次数的错误码 */
        }

        (void)osDelay(1); /* 等待10ms后判断状态 */

        if (g_wifi_state == WIFI_STA_SAMPLE_INIT) {
            printf("%s::Attempt %d: Scan start!\r\n", WIFI_STA_SAMPLE_LOG, total_attempts + 1);
            g_wifi_state = WIFI_STA_SAMPLE_SCANING;
            /* 启动STA扫描 */
            if (wifi_sta_scan() != 0) {
                g_wifi_state = WIFI_STA_SAMPLE_INIT;
                total_attempts++; /* 扫描失败，计数+1 */
                continue;
            }
        } else if (g_wifi_state == WIFI_STA_SAMPLE_SCAN_DONE) {
            /* 获取待连接的网络 */
            if (example_get_match_network(&expected_bss, ssid, psk) != 0) {
                printf("%s::Attempt %d: Do not find AP, retry...\r\n", 
                      WIFI_STA_SAMPLE_LOG, total_attempts + 1);
                g_wifi_state = WIFI_STA_SAMPLE_INIT;
                total_attempts++; /* 未找到AP，计数+1 */
                continue;
            }
            g_wifi_state = WIFI_STA_SAMPLE_FOUND_TARGET;
        } else if (g_wifi_state == WIFI_STA_SAMPLE_FOUND_TARGET) {
            printf("%s::Attempt %d: Connect start.\r\n", WIFI_STA_SAMPLE_LOG, total_attempts + 1);
            g_wifi_state = WIFI_STA_SAMPLE_CONNECTING;
            /* 启动连接 */
            if (wifi_sta_connect(&expected_bss) != 0) {
                g_wifi_state = WIFI_STA_SAMPLE_INIT;
                total_attempts++; /* 连接请求失败，计数+1 */
                continue;
            }
        } else if (g_wifi_state == WIFI_STA_SAMPLE_CONNECT_DONE) {
            printf("%s::Attempt %d: DHCP start.\r\n", WIFI_STA_SAMPLE_LOG, total_attempts + 1);
            g_wifi_state = WIFI_STA_SAMPLE_GET_IP;
            netif_p = netifapi_netif_find(ifname);
            if (netif_p == TD_NULL || netifapi_dhcp_start(netif_p) != 0) {
                printf("%s::Attempt %d: DHCP start failed, retry...\r\n", 
                      WIFI_STA_SAMPLE_LOG, total_attempts + 1);
                g_wifi_state = WIFI_STA_SAMPLE_INIT;
                total_attempts++; /* DHCP启动失败，计数+1 */
                continue;
            }
        } else if (g_wifi_state == WIFI_STA_SAMPLE_GET_IP) {
            if (example_check_dhcp_status(netif_p, &wait_count) == 0) {
                netifapi_netif_common(netif_p, dhcp_clients_info_show, NULL);
                printf("%s::Connected successfully after %d attempts.\r\n", 
                      WIFI_STA_SAMPLE_LOG, total_attempts + 1);
                return 0; /* 成功连接 */
            }
            /* DHCP等待超时，计入尝试次数 */
            if (wait_count >= 100) { /* 假设等待100*10ms=1s超时 */
                printf("%s::Attempt %d: DHCP timeout, retry...\r\n", 
                      WIFI_STA_SAMPLE_LOG, total_attempts + 1);
                g_wifi_state = WIFI_STA_SAMPLE_INIT;
                total_attempts++;
                wait_count = 0; /* 重置等待计数器 */
            } else {
                wait_count++;
            }
        }
    } while (1);
}

int func_wifi_connect
    (
    const char *ssid,
    const char *psk
    )
{
    /* 注册事件回调 */
    if (wifi_register_event_cb(&wifi_event_cb) != 0) {
        printf("%s::wifi_event_cb register fail.\r\n", WIFI_STA_SAMPLE_LOG);
        return -1;
    }
    printf("%s::wifi_event_cb register succ.\r\n", WIFI_STA_SAMPLE_LOG);

    /* 等待wifi初始化完成 */
    while (wifi_is_wifi_inited() == 0) {
        (void)osDelay(10); /* 1: 等待100ms后判断状态 */
    }
    printf("%s::wifi init succ.\r\n", WIFI_STA_SAMPLE_LOG);

    if (example_sta_function_with_retry(ssid, psk) != 0) {
        printf("%s::example_sta_function fail.\r\n", WIFI_STA_SAMPLE_LOG);
        return -1;
    }
    return 0;
}

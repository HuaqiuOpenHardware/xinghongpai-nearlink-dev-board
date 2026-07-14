#include "wifi_sta.h"

#include <stdio.h>
#include "wifi_device_config.h"
#include "wifi_hotspot_config.h"
#include "lwip/ip4_addr.h"
#include "lwip/netifapi.h"
#include "wifi_hotspot.h"

#define SERVER_PORT 8080

errcode_t func_wifi_sta_enable
    (
    void
    )
{
    /* SoftAp接口的信息 */
    char ssid[WIFI_MAX_SSID_LEN] = "WS63";
    char pre_shared_key[WIFI_MAX_KEY_LEN] = "YOUR_AP_PASSWORD";
    softap_config_stru hapd_conf = {0};
    softap_config_advance_stru config = {0};
    char ifname[WIFI_IFNAME_MAX_SIZE + 1] = "ap0"; /* 创建的SoftAp接口名 */
    struct netif *netif_p = NULL;
    ip4_addr_t st_gw;
    ip4_addr_t st_ipaddr;
    ip4_addr_t st_netmask;
    
    // 修正网关配置：通常SoftAP的网关应设为自身IP（192.168.5.1），原配置192.168.5.2可能不合理
    IP4_ADDR(&st_ipaddr, 192, 168, 5, 1);    /* 设备自身IP */
    IP4_ADDR(&st_netmask, 255, 255, 255, 0); /* 子网掩码 */
    IP4_ADDR(&st_gw, 192, 168, 5, 1);        /* 网关设为自身IP（客户端默认网关指向设备） */

    /* 配置SoftAp基本参数 */
    (void)memcpy_s(hapd_conf.ssid, sizeof(hapd_conf.ssid), ssid, sizeof(ssid));
    (void)memcpy_s(hapd_conf.pre_shared_key, WIFI_MAX_KEY_LEN, pre_shared_key, WIFI_MAX_KEY_LEN);
    hapd_conf.security_type = 3; /* WPA_WPA2_PSK加密 */
    hapd_conf.channel_num = 6;   /* 工作信道6 */
    hapd_conf.wifi_psk_type = 0;

    /* 配置SoftAp高级参数 */
    config.beacon_interval = 100; /* Beacon周期100ms */
    config.dtim_period = 2;       /* DTIM周期2 */
    config.gi = 0;                /* 关闭short GI */
    config.group_rekey = 86400;   /* 组播秘钥更新时间1天 */
    config.protocol_mode = 4;     /* 支持802.11b/g/n/ax */
    config.hidden_ssid_flag = 1;  /* 不隐藏SSID */
    if (wifi_set_softap_config_advance(&config) != 0) {
        return ERRCODE_FAIL;
    }
    
    /* 启动SoftAp接口 */
    if (wifi_softap_enable(&hapd_conf) != 0) {
        return ERRCODE_FAIL;
    }
    
    /* 配置网络参数和DHCP服务器 */
    netif_p = netif_find(ifname);
    if (netif_p == NULL) {
        (void)wifi_softap_disable();
        return ERRCODE_FAIL;
    }
    if (netifapi_netif_set_addr(netif_p, &st_ipaddr, &st_netmask, &st_gw) != 0) {
        (void)wifi_softap_disable();
        return ERRCODE_FAIL;
    }
    if (netifapi_dhcps_start(netif_p, NULL, 0) != 0) {
        (void)wifi_softap_disable();
        return ERRCODE_FAIL;
    }
    printf("SoftAp start success. IP: 192.168.5.1, Port: %d\r\n", SERVER_PORT);
    return ERRCODE_SUCC;
}

errcode_t func_wifi_sta_disable
    (
    void
    )
{
    char ifname[WIFI_IFNAME_MAX_SIZE + 1] = "ap0"; /* SoftAp接口名 */
    struct netif *netif_p = NULL;
    
    /* 查找SoftAP网络接口 */
    netif_p = netif_find(ifname);
    if (netif_p == NULL) {
        printf("SoftAP interface not found\n");
        return ERRCODE_FAIL;
    }
    
    /* 停止DHCP服务器 */
    if (netifapi_dhcps_stop(netif_p) != 0) {
        printf("Failed to stop DHCP server\n");
        /* 即使DHCP停止失败，也继续尝试禁用SoftAP */
    }
    
    /* 禁用SoftAP接口 */
    if (wifi_softap_disable() != 0) {
        printf("Failed to disable SoftAP\n");
        return ERRCODE_FAIL;
    }
    
    printf("SoftAP disabled successfully\n");
    return ERRCODE_SUCC;
}

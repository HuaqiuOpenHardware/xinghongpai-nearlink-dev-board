/**
 * History: \n
 * 2025-10-17, Create file. \n
 */
#include <stdio.h>
#include "app_init.h"
#include "common_def.h"

#include "osal_task.h"
#include "gpio.h"
#include "pinctrl.h"
#include "i2c.h"
#include "lwip/sockets.h"
#include "wifi_hotspot.h"
#include "cJSON.h"
#include "upg_porting.h"

#include "./aht20/aht20.h"
#include "./wifi/wifi_connect.h"

#define TEST_THREAD_STACK_SIZE      0x1000
#define TEST_THREAD_PRIO            17

#define SCL_PIN     17
#define SDA_PIN     18
#define PIN_MODE    2

#define SSID                    "YOUR_WIFI_SSID"
#define PASSWORD                "YOUR_WIFI_PASSWORD"
#define BOARDCAST_PORT          8000

static void *test_thread(const char *arg)
{
    unused(arg);
    printf("\r\n205_temperature\r\n");

    func_wifi_connect(SSID, PASSWORD);

    /* 广播设备tcp服务器的ip地址和端口 */
    // 在sock_fd 进行监听
    int sock_fd;
    int broadcastEnable = 1;

    printf("create socket start!\r\n");
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("create socket failed!\r\n");
        lwip_close(sock_fd);
    }
    printf("create socket end!\r\n");

    // 设置socket为广播模式
    if (setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) == -1)
    {
        printf("setsockopt(SO_BROADCAST) failed!\r\n");
        lwip_close(sock_fd);
    }
    printf("set broadcast option success!\r\n");

    // 服务器的地址信息
    struct sockaddr_in send_addr;
    socklen_t addr_length = sizeof(send_addr);

    // 初始化广播地址
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons(BOARDCAST_PORT);
    send_addr.sin_addr.s_addr = inet_addr("255.255.255.255");
    addr_length = sizeof(send_addr);

    osal_msleep(500);

    // char send_data[128] = {0};
    char send_data[128] = "hello";

    uapi_pin_init();
    uapi_gpio_init();

    uint32_t baudrate = 400000;
    uint32_t hscode = 0x0;
    uapi_pin_set_mode(SCL_PIN, PIN_MODE);
    uapi_pin_set_mode(SDA_PIN, PIN_MODE);
    errcode_t ret = uapi_i2c_master_init(0, baudrate, hscode);
    if (ret != 0)
    {
        printf("uapi_i2c_master_init() failed, ret = %0x\r\n", ret);
    }

    // 检查AHT20的校准使能状态
    while (AHT20_Calibrate() != 0)
    {
        printf("AHT20_Calibrate() failed\r\n");
        osal_msleep(500); // 500ms后再判断设备是否复位成功
    }

    uint32_t retval = 0;
    float temp = 0;
    float humi = 0;
    char temp_str[20];  // 存储温度字符串
    char humi_str[20];  // 存储湿度字符串
    
    while (1)
    {
        retval = AHT20_StartMeasure();
        printf("AHT20_StartMeasure: %d\r\n", retval);
        retval = AHT20_GetMeasureResult(&temp, &humi);
        if (retval != 0)
        {
            printf("AHT20_GetMeasureResult() failed, ret = %d\r\n", retval);
        }
        else
        {
            // // 转换为字符串（保留2位小数）
            // sprintf(temp_str, "温度: %.2f °C", temp);
            // sprintf(humi_str, "湿度: %.2f %%", humi);
            // // 输出字符串
            // printf("%s, %s\r\n", temp_str, humi_str);

            // 转换为字符串（保留2位小数）
            sprintf(send_data, "temp: %.2f °C, humi: %.2f %%", temp, humi);

            // // 发送数据到广播地址
            // printf("sendto broadcast start! Data: %s\r\n", send_data);
            if (sendto(sock_fd, send_data, strlen(send_data), 0, (struct sockaddr *)&send_addr, addr_length) == -1) 
            {
                printf("sendto broadcast failed!\r\n");
            }
            else 
            {
                printf("sendto broadcast end!\r\n");
            }
        }

        osal_msleep(1000);
    }
    
    return NULL;
}


static void test_entry(void)
{
    osal_task *test_handle = NULL;
    osal_kthread_lock();
    test_handle = osal_kthread_create((osal_kthread_handler)test_thread, 0, "test_thread", TEST_THREAD_STACK_SIZE);
    if (test_handle != NULL)
    {
        osal_kthread_set_priority(test_handle, TEST_THREAD_PRIO);
    }
    osal_kthread_unlock();
}

app_run(test_entry);

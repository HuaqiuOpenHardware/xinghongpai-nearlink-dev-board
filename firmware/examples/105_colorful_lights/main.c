/**
 * History: \n
 * 2025-10-17, Create file. \n
 */
#include <stdio.h>

#include <unistd.h>

#include "cmsis_os2.h"
#include "ohos_init.h"
#include "iot_gpio.h"

#include "gpio.h"
#include "pinctrl.h"

#include "./rgb/rgb_demo.h"
#include "./sr602/sr602_demo.h"

#define TEST_THREAD_STACK_SIZE      0x1000
#define TEST_THREAD_PRIO            17

static void *test_thread(const char *arg)
{
    unused(arg);

    printf("\r\n105_colorful_lights\r\n");

    // 配置RGB LED
    rgb_led_config_t led_config = {
        .r_pin = GPIO_02,           // 红色控制引脚
        .g_pin = GPIO_03,           // 绿色控制引脚
        .b_pin = GPIO_06,           // 蓝色控制引脚
        .is_common_anode = false,   // 共阴极LED
    };

    func_rgb_led_init(&led_config);

    func_sr602_init(GPIO_08);
    
    while (1)
    {
        if (func_sr602_is_motion_detected() == true)
        {
            func_rgb_led_set_color(COLOR_RED);
        }
        else
        {
            func_rgb_led_set_color(COLOR_GREEN);
        }
        osal_msleep(100);

        // printf("hello world\r\n");

        // // 设置颜色为红色
        // func_rgb_led_set_color(COLOR_RED);
        // osal_msleep(1000);
        
        // // 设置颜色为绿色
        // func_rgb_led_set_color(COLOR_GREEN);
        // osal_msleep(1000);
        
        // // 设置颜色为蓝色
        // func_rgb_led_set_color(COLOR_BLUE);
        // osal_msleep(1000);
        
        // // 设置颜色为黄色
        // func_rgb_led_set_color(COLOR_YELLOW);
        // osal_msleep(1000);
        
        // // 关闭LED
        // func_rgb_led_off();
        // osal_msleep(1000);
        
        // // 开启LED（白色）
        // func_rgb_led_on();
        // osal_msleep(1000);
    }
    
    return NULL;
}


static void test_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "test_thread";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 2048;
    attr.priority = 25;

    if (osThreadNew((osThreadFunc_t) test_thread, NULL, &attr) == NULL) {
        printf("[test_thread] Falied to create GpioTask!\n");
    }
}

SYS_RUN(test_entry);

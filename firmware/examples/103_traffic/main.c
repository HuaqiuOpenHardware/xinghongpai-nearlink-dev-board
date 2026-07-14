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
#include "systick.h"
#include "watchdog.h"

#include "./traffic_light/traffic_light.h"
#include "./key/key_demo.h"
#include "./segment_display/segment_display.h"

#define SEGMENT_THREAD_STACK_SIZE      0x1000
#define SEGMENT_THREAD_PRIO            18

static void *segment_thread(const char *arg)
{
    unused(arg);

    // 配置数码管引脚
    segment_display_config_t segment_display_config = {
        .seg_a = GPIO_11,
        .seg_b = GPIO_01,
        .seg_c = GPIO_06,
        .seg_d = GPIO_03,
        .seg_e = GPIO_14,
        .seg_f = GPIO_12,
        .seg_g = GPIO_07,
        .seg_dp = GPIO_10,
        .digit1 = GPIO_04,  // 十位
        .digit2 = GPIO_02   // 个位
    };
    // 初始化数码管
    func_segment_display_init(&segment_display_config);
    func_segment_display_show_number(00);

    uapi_systick_init();

    uint16_t count = 0;

    while (1)
    {
        func_segment_display_process(2);
        uapi_systick_delay_ms(2);

        if (++count >= 500)
        {
            count = 0;
            static uint8_t number = 0;
            func_segment_display_show_number(number);
            number = (number + 1) % 100;
        }
    }
    return NULL;
}

#define TEST_THREAD_STACK_SIZE      0x1000
#define TEST_THREAD_PRIO            17

static void *test_thread(const char *arg)
{
    unused(arg);

    printf("\r\n103_traffic\r\n");
    sleep(1); //等待uart2（gpio7,gpio8）初始化完成后，再设置GPIO7为gpio功能

    // 配置交通灯
    traffic_light_config_t traffic_ligtht_config = {
        .red_pin = GPIO_08,
        .yellow_pin = GPIO_09,
        .green_pin = GPIO_05
    };
    func_traffic_light_init(&traffic_ligtht_config);

    key_config_t key_config = {
        .pin = GPIO_00,
        .active_level = GPIO_LEVEL_LOW, // 低电平有效
        .debounce_time = 20,            // 20ms消抖
    };
    func_key_register(&key_config);

    osThreadAttr_t attr;

    attr.name = "segment_thread";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 2048;
    attr.priority = 25;

    if (osThreadNew((osThreadFunc_t) segment_thread, NULL, &attr) == NULL) {
        printf("[segment_thread] Falied to create GpioTask!\n");
    }

    uint16_t count_watchdog = 0;

    while (1)
    {
        func_key_process(10);
        osDelay(10);

        // 检查按键事件
        key_event_t event = func_key_get_event(GPIO_00);

        if (event == KEY_EVENT_PRESS)
        {
            printf("key pressed\r\n");
            func_traffic_light_set_all(true, true, true);
        } 
        else if (event == KEY_EVENT_RELEASE) 
        {
            printf("key released\r\n");
            func_traffic_light_off();
        }

        //喂看门狗
        count_watchdog++;
        if (count_watchdog >= 100)
        {
            uapi_watchdog_kick(); 
        }
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

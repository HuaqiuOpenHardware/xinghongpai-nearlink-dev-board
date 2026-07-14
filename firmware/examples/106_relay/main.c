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

#include "./relay/relay_demo.h"

#define TEST_THREAD_STACK_SIZE      0x1000
#define TEST_THREAD_PRIO            17

static void *test_thread(const char *arg)
{
    unused(arg);

    printf("\r\n106_relay\r\n");
    osal_msleep(1000); //等待uart2（gpio7,gpio8）初始化完成后，再设置GPIO7为gpio功能

    func_relay_init();
    
    while (1)
    {
        func_relay_on();
        printf("func_relay_on\r\n");
        osal_msleep(1000);

        func_relay_off();
        printf("func_relay_off\r\n");
        osal_msleep(1000);
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

/**
 * History: \n
 * 2025-10-17, Create file. \n
 */
#include <stdio.h>

#include <unistd.h>

#include "cmsis_os2.h"
#include "ohos_init.h"
#include "iot_gpio.h"

#include "./fan/fan_demo.h"



static void *test_thread(const char *arg)
{
    arg = arg;

    printf("\r\n101_fan\r\n");
    sleep(1000); //等待uart2（gpio7,gpio8）初始化完成后，再设置GPIO7为gpio功能

    func_fan_init();
    // func_fan_forward();
    func_fan_backward();
    
    while (1)
    {
        printf("hello 101_fan\r\n");
        sleep(1);
    }
    
    return NULL;
}


static void test_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "FunTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 2048;
    attr.priority = 25;

    if (osThreadNew((osThreadFunc_t) test_thread, NULL, &attr) == NULL) {
        printf("[FunTask] Falied to create GpioTask!\n");
    }
}

SYS_RUN(test_entry);

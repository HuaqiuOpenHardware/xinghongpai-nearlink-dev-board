
#include <stdio.h>

#include <unistd.h>

#include "cmsis_os2.h"
#include "ohos_init.h"
#include "iot_gpio.h"

//LED灯的引脚是IO14
#define LED_TASK_GPIO 14


static void *gpio_test_task(void *data)
{
    (void) data;

    //初始化IO引脚
    IoTGpioInit(LED_TASK_GPIO);

    //设置为输出引脚
    IoTGpioSetDir(LED_TASK_GPIO, IOT_GPIO_DIR_OUT);

    while (1) {
        printf(" LED_SPARK! \n");

        //输出0低电平
        IoTGpioSetOutputVal(LED_TASK_GPIO, 0);
        osDelay(50);

        //输出1高电平
        IoTGpioSetOutputVal(LED_TASK_GPIO, 1);
        osDelay(50);
    }
    return NULL;
}

static void GpioExampleEntry(void)
{
    osThreadAttr_t attr;

    attr.name = "GpioTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 2048;
    attr.priority = 25;

    if (osThreadNew((osThreadFunc_t) gpio_test_task, NULL, &attr) == NULL) {
        printf("[GpioExample] Falied to create GpioTask!\n");
    }
}

SYS_RUN(GpioExampleEntry);

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
#include "adc.h"
#include "adc_porting.h"


#define TEST_THREAD_STACK_SIZE      0x1000
#define TEST_THREAD_PRIO            17

#define DELAY_2000MS                    2000
#define CYCLES                          10

#define CONFIG_ADC_CHANNEL              0

static void *test_thread(const char *arg)
{
    unused(arg);

    printf("\r\n302_smoke\r\n");
    osal_msleep(1000); //等待uart2（gpio7,gpio8）初始化完成后，再设置GPIO7为gpio功能

    uapi_pin_init();
    uapi_gpio_init();
    
    uapi_adc_init(ADC_CLOCK_NONE);
    uint8_t adc_channel = CONFIG_ADC_CHANNEL;
    uint16_t voltage = 0;
    uint32_t cnt = 0;
    while (cnt++ < CYCLES)
    {
        adc_port_read(adc_channel, &voltage);
        printf("mq2 voltage: %d mv\r\n", voltage);
        osal_msleep(DELAY_2000MS);
    }
    /* 当前测量的电压值和实际值可能有较大差别，请确认是否有分压电阻，如果有分压电阻，则差别符合预期 */
    uapi_adc_deinit();
    
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

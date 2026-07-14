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

#include "./flame/flame_demo.h"

#define TEST_THREAD_STACK_SIZE      0x1000
#define TEST_THREAD_PRIO            17

static void *test_thread(const char *arg)
{
    unused(arg);

    printf("\r\n102_flame\r\n");

    func_flame_init(GPIO_06);
    
    while (1)
    {
        if (func_flame_is_detected() == true)
        {
            printf("func_flame_is_detected() is true\r\n");
        }
        else
        {
            printf("func_flame_is_detected() is false\r\n");
        }
        osal_msleep(100);
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

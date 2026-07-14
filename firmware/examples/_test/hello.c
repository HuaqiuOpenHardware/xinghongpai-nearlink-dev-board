#include <stdio.h>

#include "app_init.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "osal_addr.h"

#define DEMO_TASK_PRIO 24
#define DEMO_TASK_STACK_SIZE 0x1000

static void *demo_task(void)
{    
    while (1)
    {
        osal_printk("\r\nhello world!\r\n");

        osal_msleep(1000);
    }
    return NULL;
}

static void demo_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)demo_task, 0, "demo_task", DEMO_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, DEMO_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

app_run(demo_entry);

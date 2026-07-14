#ifndef _RELAY_DEMO_H
#define _RELAY_DEMO_H

#include <stdio.h>

#include <unistd.h>

#include "cmsis_os2.h"
#include "ohos_init.h"
#include "iot_gpio.h"

#include "gpio.h"
#include "pinctrl.h"

// 继电器控制引脚宏定义（需根据实际硬件电路的引脚编号修改）
#define PIN_RELAY    GPIO_07


void func_relay_init(void);    // 初始化继电器控制引脚
void func_relay_on(void);      // 控制继电器吸合（打开）
void func_relay_off(void);     // 控制继电器断开（关闭）


#endif
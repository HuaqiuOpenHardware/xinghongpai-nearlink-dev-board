
#ifndef _FAN_DEMO_H
#define _FAN_DEMO_H

#include "gpio.h"
#include "pinctrl.h"
#include "iot_gpio.h"

#define PIN_FORWARD     7
#define PIN_BACKWARD    5

void func_fan_init(void);

void func_fan_forward(void);

void func_fan_backward(void);

#endif

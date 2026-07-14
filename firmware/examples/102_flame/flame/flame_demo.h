#ifndef _FLAME_DEMO_H
#define _FLAME_DEMO_H

#include "gpio.h"
#include "pinctrl.h"


/**
 * @brief   初始化火焰传感器模块
 * @param   pin 传感器输出引脚
 * @retval  ERRCODE_SUCC 成功
 * @retval  其他错误码 失败
 */
errcode_t func_flame_init
    (
    pin_t pin
    );

/**
 * @brief   获取当前火焰检测状态
 * @return  true:检测到火焰, false:未检测到火焰
 */
bool func_flame_is_detected
    (
    void
    );


#endif 
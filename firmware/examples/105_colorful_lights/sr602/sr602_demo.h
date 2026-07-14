#ifndef _SR602_DEMO_H
#define _SR602_DEMO_H

#include "gpio.h"
#include "pinctrl.h"

/**
 * @brief 初始化SR602人体感应模块
 * @param pin 感应输出引脚
 * @retval ERRCODE_SUCC 成功
 * @retval 其他错误码 失败
 */
errcode_t func_sr602_init
    (
    pin_t pin
    );

/**
 * @brief 获取当前人体检测状态
 * @return true: 检测到人体, false: 未检测到人体
 */
bool func_sr602_is_motion_detected
    (
    void
    );



#endif 
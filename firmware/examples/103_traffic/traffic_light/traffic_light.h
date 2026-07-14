#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <stdio.h>

#include <unistd.h>

#include "cmsis_os2.h"
#include "ohos_init.h"
#include "iot_gpio.h"

#include "gpio.h"
#include "pinctrl.h"

// 交通灯引脚配置
typedef struct {
    pin_t red_pin;     // 红灯引脚
    pin_t yellow_pin;  // 黄灯引脚
    pin_t green_pin;   // 绿灯引脚
} traffic_light_config_t;

/**
 * @brief 初始化交通灯模块
 * @param config 交通灯配置
 * @retval ERRCODE_SUCC 成功
 * @retval 其他错误码 失败
 */
errcode_t func_traffic_light_init
    (
    const traffic_light_config_t *config_ptr
    );

/**
 * @brief 设置红灯状态
 * @param on true:开启, false:关闭
 * @retval ERRCODE_SUCC 成功
 * @retval 其他错误码 失败
 */
errcode_t func_traffic_light_set_red
    (
    bool on
    );

/**
 * @brief 设置黄灯状态
 * @param on true:开启, false:关闭
 * @retval ERRCODE_SUCC 成功
 * @retval 其他错误码 失败
 */
errcode_t func_traffic_light_set_yellow
    (
    bool on
    );

/**
 * @brief 设置绿灯状态
 * @param on true:开启, false:关闭
 * @retval ERRCODE_SUCC 成功
 * @retval 其他错误码 失败
 */
errcode_t func_traffic_light_set_green
    (
    bool on
    );

/**
 * @brief 设置所有灯状态
 * @param red_on 红灯状态
 * @param yellow_on 黄灯状态
 * @param green_on 绿灯状态
 * @retval ERRCODE_SUCC 成功
 * @retval 其他错误码 失败
 */
errcode_t func_traffic_light_set_all
    (
    bool red_on, 
    bool yellow_on, 
    bool green_on
    );

/**
 * @brief 关闭所有灯
 * @retval ERRCODE_SUCC 成功
 * @retval 其他错误码 失败
 */
errcode_t func_traffic_light_off
    (
    void
    );
    

#endif // TRAFFIC_LIGHT_H
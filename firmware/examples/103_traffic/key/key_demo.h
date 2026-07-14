#ifndef _KEY_DEMO_H
#define _KEY_DEMO_H

#include <stdio.h>

#include <unistd.h>

#include "cmsis_os2.h"
#include "ohos_init.h"
#include "iot_gpio.h"

#include "gpio.h"
#include "pinctrl.h"

// 按键事件类型
typedef enum {
    KEY_EVENT_NONE = 0,      // 无事件
    KEY_EVENT_PRESS,         // 按下事件
    KEY_EVENT_RELEASE,       // 释放事件
} key_event_t;

// 按键配置结构
typedef struct {
    pin_t pin;                  // 按键引脚
    gpio_level_t active_level;  // 有效电平（按下时的电平）
    uint32_t debounce_time;     // 消抖时间(ms)
} key_config_t;

/**
 * @brief 初始化按键模块
 * @retval ERRCODE_SUCC 成功
 * @retval 其他错误码 失败
 */
errcode_t func_key_init
    (
    void
    );

/**
 * @brief 注册一个按键
 * @param config 按键配置
 * @retval ERRCODE_SUCC 成功
 * @retval 其他错误码 失败
 */
errcode_t func_key_register
    (
    const key_config_t *config
    );

/**
 * @brief 按键处理函数，需要在主循环中定期调用
 * @param elapsed_ms 从上一次调用到现在的经过时间(ms)
 */
void func_key_process
    (
    uint32_t elapsed_ms
    );

/**
 * @brief 获取按键事件
 * @param pin 按键引脚
 * @return 按键事件
 */
key_event_t func_key_get_event
    (
    pin_t pin
    );

/**
 * @brief 清除按键事件
 * @param pin 按键引脚
 */
void func_key_clear_event
    (
    pin_t pin
    );

#endif
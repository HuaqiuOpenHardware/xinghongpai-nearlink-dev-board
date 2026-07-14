#ifndef _RGB_DEMO_H
#define _RGB_DEMO_H

#include <stdio.h>

#include <unistd.h>

#include "cmsis_os2.h"
#include "ohos_init.h"
#include "iot_gpio.h"

#include "gpio.h"
#include "pinctrl.h"


// RGB颜色结构
typedef struct {
    uint8_t r;  // 红色分量 (0-255)
    uint8_t g;  // 绿色分量 (0-255)
    uint8_t b;  // 蓝色分量 (0-255)
} rgb_color_t;

// RGB LED配置
typedef struct {
    pin_t r_pin;            // 红色控制引脚
    pin_t g_pin;            // 绿色控制引脚
    pin_t b_pin;            // 蓝色控制引脚
    bool is_common_anode;   // 是否为共阳极LED
} rgb_led_config_t;

/**
 * @brief 初始化RGB LED
 * @param config RGB LED配置
 * @retval ERRCODE_SUCC 成功
 * @retval 其他错误码 失败
 */
errcode_t func_rgb_led_init
    (
    const rgb_led_config_t *config
    );

/**
 * @brief 设置RGB LED颜色
 * @param color 颜色值
 * @retval ERRCODE_SUCC 成功
 * @retval 其他错误码 失败
 */
errcode_t func_rgb_led_set_color
    (
    rgb_color_t color
    );

/**
 * @brief 开启RGB LED
 * @retval ERRCODE_SUCC 成功
 * @retval 其他错误码 失败
 */
errcode_t func_rgb_led_on
    (
    void
    );

/**
 * @brief 关闭RGB LED
 * @retval ERRCODE_SUCC 成功
 * @retval 其他错误码 失败
 */
errcode_t func_rgb_led_off
    (
    void
    );

// 预定义常用颜色
extern const rgb_color_t COLOR_RED;
extern const rgb_color_t COLOR_GREEN;
extern const rgb_color_t COLOR_BLUE;
extern const rgb_color_t COLOR_WHITE;
extern const rgb_color_t COLOR_BLACK;
extern const rgb_color_t COLOR_YELLOW;
extern const rgb_color_t COLOR_CYAN;
extern const rgb_color_t COLOR_MAGENTA;


#endif
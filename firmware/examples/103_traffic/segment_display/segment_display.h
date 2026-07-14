#ifndef _SEGMENT_DISPLAY_H
#define _SEGMENT_DISPLAY_H

#include <stdio.h>

#include <unistd.h>

#include "cmsis_os2.h"
#include "ohos_init.h"
#include "iot_gpio.h"

#include "gpio.h"
#include "pinctrl.h"


// 数码管引脚配置
typedef struct {
    // 段选引脚 (a, b, c, d, e, f, g, dp)
    pin_t seg_a;
    pin_t seg_b;
    pin_t seg_c;
    pin_t seg_d;
    pin_t seg_e;
    pin_t seg_f;
    pin_t seg_g;
    pin_t seg_dp;
    
    // 位选引脚 (共阳数码管的公共端)
    pin_t digit1;  // 十位
    pin_t digit2;  // 个位
} segment_display_config_t;

/**
 * @brief 初始化数码管显示模块
 * @param config 数码管配置
 * @retval ERRCODE_SUCC 成功
 * @retval 其他错误码 失败
 */
errcode_t func_segment_display_init
    (
    const segment_display_config_t *config
    );

/**
 * @brief 显示数字
 * @param number 要显示的数字 (0-99)
 * @retval ERRCODE_SUCC 成功
 * @retval 其他错误码 失败
 */
errcode_t func_segment_display_show_number
    (
    uint8_t number
    );

/**
 * @brief 显示特定段
 * @param digit 位选 (1: 十位, 2: 个位)
 * @param segments 段选掩码 (每位对应一个段: a,b,c,d,e,f,g,dp)
 * @retval ERRCODE_SUCC 成功
 * @retval 其他错误码 失败
 */
errcode_t func_segment_display_show_segments
    (
    uint8_t digit, 
    uint8_t segments
    );

/**
 * @brief 清除显示
 * @retval ERRCODE_SUCC 成功
 * @retval 其他错误码 失败
 */
errcode_t func_segment_display_clear
    (
    void
    );

/**
 * @brief 显示处理函数，需要在主循环中定期调用
 * @param elapsed_ms 从上一次调用到现在的经过时间(ms)
 */
void func_segment_display_process
    (
    uint32_t elapsed_ms
    );


#endif // _SEGMENT_DISPLAY_H
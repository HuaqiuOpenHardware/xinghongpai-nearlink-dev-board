#include "sr602_demo.h"

// 模块状态
static pin_t output_pin = PIN_NONE;

errcode_t func_sr602_init
    (
    pin_t pin
    )
{
    uapi_pin_init();
    uapi_gpio_init();

    // 初始化GPIO引脚为输入
    uapi_pin_set_mode(pin, PIN_MODE_0);
    errcode_t ret = uapi_gpio_set_dir(pin, GPIO_DIRECTION_INPUT);
    if (ret != ERRCODE_SUCC)
    {
        return ret;
    }
    
    // 保存引脚配置
    output_pin = pin;
    
    return ERRCODE_SUCC;
}

bool func_sr602_is_motion_detected
    (
    void
    )
{
    if (output_pin == PIN_NONE)
    {
        return false; // 未初始化
    }
    
    // 读取引脚状态，高电平表示检测到人体
    return (uapi_gpio_get_val(output_pin) == GPIO_LEVEL_HIGH);
}
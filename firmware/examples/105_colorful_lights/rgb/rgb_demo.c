#include "rgb_demo.h"

// 全局配置
static rgb_led_config_t led_config;
static bool is_initialized = false;

// 预定义颜色
const rgb_color_t COLOR_RED = {255, 0, 0};
const rgb_color_t COLOR_GREEN = {0, 255, 0};
const rgb_color_t COLOR_BLUE = {0, 0, 255};
const rgb_color_t COLOR_WHITE = {255, 255, 255};
const rgb_color_t COLOR_BLACK = {0, 0, 0};
const rgb_color_t COLOR_YELLOW = {255, 255, 0};
const rgb_color_t COLOR_CYAN = {0, 255, 255};
const rgb_color_t COLOR_MAGENTA = {255, 0, 255};

// 设置单个引脚的PWM值（内部函数）
static void set_pin_value
    (
    pin_t   pin, 
    uint8_t value, 
    bool    invert
)
{
    if (invert)
    {
        value = 255 - value;
    }

    // 简化实现：使用GPIO模拟PWM
    // 注意：这只是一个简化实现，实际应用中应该使用硬件PWM
    if (value > 127)
    {
        IoTGpioSetOutputVal(pin, GPIO_LEVEL_HIGH);
    } 
    else 
    {
        IoTGpioSetOutputVal(pin, GPIO_LEVEL_LOW);
    }
}

errcode_t func_rgb_led_init
    (
    const rgb_led_config_t *config
    ) 
{
    if (is_initialized)
    {
        return ERRCODE_SUCC; // 已经初始化过
    }
    
    // 保存配置
    memcpy(&led_config, config, sizeof(rgb_led_config_t));
    
    IoTGpioInit(config->r_pin);
    IoTGpioInit(config->g_pin);
    IoTGpioInit(config->b_pin);

    // 初始化GPIO引脚为输出
    uapi_pin_set_mode(config->r_pin, PIN_MODE_0);
    errcode_t ret = IoTGpioSetDir(config->r_pin, GPIO_DIRECTION_OUTPUT);
    if (ret != ERRCODE_SUCC)
    {
        return ret;
    }
    
    uapi_pin_set_mode(config->g_pin, PIN_MODE_0);
    ret = IoTGpioSetDir(config->g_pin, GPIO_DIRECTION_OUTPUT);
    if (ret != ERRCODE_SUCC)
    {
        return ret;
    }
    
    uapi_pin_set_mode(config->b_pin, PIN_MODE_0);
    ret = IoTGpioSetDir(config->b_pin, GPIO_DIRECTION_OUTPUT);
    if (ret != ERRCODE_SUCC)
    {
        return ret;
    }
    
    is_initialized = true;

    // 初始状态关闭LED
    func_rgb_led_off();
    
    return ERRCODE_SUCC;
}

errcode_t func_rgb_led_set_color
    (
    rgb_color_t color
    )
{
    if (!is_initialized)
    {
        return ERRCODE_FAIL; // 未初始化
    }
    
    set_pin_value(led_config.r_pin, color.r, led_config.is_common_anode);
    set_pin_value(led_config.g_pin, color.g, led_config.is_common_anode);
    set_pin_value(led_config.b_pin, color.b, led_config.is_common_anode);
    
    return ERRCODE_SUCC;
}

errcode_t func_rgb_led_on
    (
    void
    ) 
{
    if (!is_initialized)
    {
        return ERRCODE_FAIL; // 未初始化
    }
    
    // 设置为白色（全亮）
    return func_rgb_led_set_color(COLOR_WHITE);
}

errcode_t func_rgb_led_off
    (
    void
    ) 
{
    if (!is_initialized)
    {
        return ERRCODE_FAIL; // 未初始化
    }
    
    // 设置为黑色（全灭）
    return func_rgb_led_set_color(COLOR_BLACK);
}
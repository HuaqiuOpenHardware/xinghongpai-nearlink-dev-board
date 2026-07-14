#include "traffic_light.h"

// 交通灯状态
static traffic_light_config_t config;

errcode_t func_traffic_light_init
    (
    const traffic_light_config_t *config_ptr
    ) 
{
    // 保存配置
    config = *config_ptr;

    uapi_pin_init();
    uapi_gpio_init();
    
    // 初始化GPIO引脚为输出
    if (config.red_pin == GPIO_04)
    {
        uapi_pin_set_mode(config.red_pin, PIN_MODE_2);
    }
    else if (config.red_pin == GPIO_05)
    {
        uapi_pin_set_mode(config.red_pin, PIN_MODE_4);
    }
    else 
    {
        uapi_pin_set_mode(config.red_pin, HAL_PIO_FUNC_GPIO);
    }
    errcode_t ret = uapi_gpio_set_dir(config.red_pin, GPIO_DIRECTION_OUTPUT);
    if (ret != ERRCODE_SUCC) 
    {
        return ret;
    }
    
    if (config.yellow_pin == GPIO_04)
    {
        uapi_pin_set_mode(config.yellow_pin, PIN_MODE_2);
    }
    else if (config.yellow_pin == GPIO_05)
    {
        uapi_pin_set_mode(config.yellow_pin, PIN_MODE_4);
    }
    else 
    {
        uapi_pin_set_mode(config.yellow_pin, HAL_PIO_FUNC_GPIO);
    }
    ret = uapi_gpio_set_dir(config.yellow_pin, GPIO_DIRECTION_OUTPUT);
    if (ret != ERRCODE_SUCC) 
    {
        return ret;
    }
    
    if (config.green_pin == GPIO_04)
    {
        uapi_pin_set_mode(config.green_pin, PIN_MODE_2);
    }
    else if (config.green_pin == GPIO_05)
    {
        uapi_pin_set_mode(config.green_pin, PIN_MODE_4);
    }
    else 
    {
        uapi_pin_set_mode(config.green_pin, HAL_PIO_FUNC_GPIO);
    }
    ret = uapi_gpio_set_dir(config.green_pin, GPIO_DIRECTION_OUTPUT);
    if (ret != ERRCODE_SUCC) 
    {
        return ret;
    }
    
    // 初始状态为关闭
    func_traffic_light_off();
    
    return ERRCODE_SUCC;
}

errcode_t func_traffic_light_set_red
    (
    bool on
    )
{
    return uapi_gpio_set_val(config.red_pin, on ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
}

errcode_t func_traffic_light_set_yellow
    (
    bool on
    ) 
{
    return uapi_gpio_set_val(config.yellow_pin, on ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
}

errcode_t func_traffic_light_set_green
    (
    bool on
    ) 
{
    return uapi_gpio_set_val(config.green_pin, on ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
}

errcode_t func_traffic_light_set_all
    (
    bool red_on, 
    bool yellow_on, 
    bool green_on
    ) 
{
    errcode_t ret;
    
    ret = func_traffic_light_set_red(red_on);
    if (ret != ERRCODE_SUCC)
    {
        return ret;
    }
    
    ret = func_traffic_light_set_yellow(yellow_on);
    if (ret != ERRCODE_SUCC)
    {
        return ret;
    }
    
    ret = func_traffic_light_set_green(green_on);
    if (ret != ERRCODE_SUCC)
    {
        return ret;
    }
    
    return ERRCODE_SUCC;
}

errcode_t func_traffic_light_off
    (
    void
    ) 
{
    return func_traffic_light_set_all(false, false, false);
}
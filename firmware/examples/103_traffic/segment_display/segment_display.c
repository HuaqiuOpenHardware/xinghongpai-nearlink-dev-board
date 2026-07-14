#include "segment_display.h"

// 数码管状态
static struct {
    segment_display_config_t config;  // 数码管配置
    uint8_t current_number;           // 当前显示的数字
    uint8_t digit1_value;             // 十位数字
    uint8_t digit2_value;             // 个位数字
    bool is_digit1_active;            // 十位是否激活
    uint32_t refresh_timer;           // 刷新计时器
} segment_display = {
    .current_number = 0,
    .digit1_value = 0,
    .digit2_value = 0,
    .is_digit1_active = true,
    .refresh_timer = 0
};

// 数字到段选的映射表 (共阳数码管，0表示点亮，1表示熄灭)
// 格式: a, b, c, d, e, f, g, dp
static const uint8_t digit_to_segments[10] = {
    0xC0, // 0: a,b,c,d,e,f点亮 -> 11000000
    0xF9, // 1: b,c点亮 -> 11111001
    0xA4, // 2: a,b,d,e,g点亮 -> 10100100
    0xB0, // 3: a,b,c,d,g点亮 -> 10110000
    0x99, // 4: b,c,f,g点亮 -> 10011001
    0x92, // 5: a,c,d,f,g点亮 -> 10010010
    0x82, // 6: a,c,d,e,f,g点亮 -> 10000010
    0xF8, // 7: a,b,c点亮 -> 11111000
    0x80, // 8: 全部点亮 -> 10000000
    0x90  // 9: a,b,c,d,f,g点亮 -> 10010000
};

errcode_t configure_pin_mode_and_dir
    (
    pin_t pin, 
    pin_mode_t default_mode
    )
{
    errcode_t ret;
    
    // 根据不同的引脚设置不同的模式
    if (pin == GPIO_04) 
    {
        uapi_pin_set_mode(pin, PIN_MODE_2);
    } 
    else if (pin == GPIO_05)
    {
        uapi_pin_set_mode(pin, PIN_MODE_4);
    } 
    else
    {
        uapi_pin_set_mode(pin, default_mode);
    }
    
    // 设置引脚方向为输出
    ret = uapi_gpio_set_dir(pin, GPIO_DIRECTION_OUTPUT);
    if (ret != ERRCODE_SUCC)
    {
        return ret;
    }

    if (pin == GPIO_14)
    {
        uapi_pin_set_ds(pin, PIN_DS_MAX);
    }
    
    return ERRCODE_SUCC;
}

// 设置单个段的状态
static void set_segment_state
    (
    pin_t pin, 
    bool state
)
{
    // 共阳数码管: 低电平点亮，高电平熄灭
    IoTGpioSetOutputVal(pin, state ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}

// 设置所有段的状态
static void set_all_segments
    (
    uint8_t segments
    ) 
{
    set_segment_state(segment_display.config.seg_a, segments & 0x01);
    set_segment_state(segment_display.config.seg_b, segments & 0x02);
    set_segment_state(segment_display.config.seg_c, segments & 0x04);
    set_segment_state(segment_display.config.seg_d, segments & 0x08);
    set_segment_state(segment_display.config.seg_e, segments & 0x10);
    set_segment_state(segment_display.config.seg_f, segments & 0x20);
    set_segment_state(segment_display.config.seg_g, segments & 0x40);
    set_segment_state(segment_display.config.seg_dp, segments & 0x80);
}

// 设置位选状态
static void set_digit_state
    (
    uint8_t digit, 
    bool state
    )
{
    // 共阳数码管: 高电平选中，低电平不选中
    if (digit == 1) 
    {
        IoTGpioSetOutputVal(segment_display.config.digit1, state ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
        IoTGpioSetOutputVal(segment_display.config.digit2, GPIO_LEVEL_LOW);
    } 
    else 
    {
        IoTGpioSetOutputVal(segment_display.config.digit1, GPIO_LEVEL_LOW);
        IoTGpioSetOutputVal(segment_display.config.digit2, state ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
    }
}

errcode_t func_segment_display_init
    (
    const segment_display_config_t *config
    )
{
    // 保存配置
    segment_display.config = *config;

    uapi_pin_init();
    uapi_gpio_init();
    
    // 初始化段选引脚为输出
    errcode_t ret = configure_pin_mode_and_dir(config->seg_a, PIN_MODE_0);
    if (ret != ERRCODE_SUCC)
    {
        return ret;
    }

    ret = configure_pin_mode_and_dir(config->seg_b, PIN_MODE_0);
    if (ret != ERRCODE_SUCC)
    {
        return ret;
    }

    ret = configure_pin_mode_and_dir(config->seg_c, PIN_MODE_0);
    if (ret != ERRCODE_SUCC)
    {
        return ret;
    }

    ret = configure_pin_mode_and_dir(config->seg_d, PIN_MODE_0);
    if (ret != ERRCODE_SUCC)
    {
        return ret;
    }

    ret = configure_pin_mode_and_dir(config->seg_e, PIN_MODE_0);
    if (ret != ERRCODE_SUCC)
    {
        return ret;
    }

    ret = configure_pin_mode_and_dir(config->seg_f, PIN_MODE_0);
    if (ret != ERRCODE_SUCC)
    {
        return ret;
    }

    ret = configure_pin_mode_and_dir(config->seg_g, PIN_MODE_0);
    if (ret != ERRCODE_SUCC)
    {
        return ret;
    }

    ret = configure_pin_mode_and_dir(config->seg_dp, PIN_MODE_0);
    if (ret != ERRCODE_SUCC)
    {
        return ret;
    }

    ret = configure_pin_mode_and_dir(config->digit1, PIN_MODE_0);
    if (ret != ERRCODE_SUCC)
    {
        return ret;
    }

    ret = configure_pin_mode_and_dir(config->digit2, PIN_MODE_0);
    if (ret != ERRCODE_SUCC)
    {
        return ret;
    }
    
    // 初始状态为关闭
    func_segment_display_clear();
    
    return ERRCODE_SUCC;
}

errcode_t func_segment_display_show_number
    (
    uint8_t number
    )
{
    if (number > 99) 
    {
        number = 99; // 限制最大显示99
    }
    
    segment_display.current_number = number;
    segment_display.digit1_value = number / 10;  // 十位
    segment_display.digit2_value = number % 10;  // 个位
    printf("%d, %d\r\n", segment_display.digit1_value, segment_display.digit2_value);
    
    return ERRCODE_SUCC;
}

errcode_t func_segment_display_show_segments
    (
    uint8_t digit, 
    uint8_t segments
    )
{
    if (digit == 1)
    {
        segment_display.digit1_value = segments;
    } 
    else
    {
        segment_display.digit2_value = segments;
    }
    
    return ERRCODE_SUCC;
}

errcode_t func_segment_display_clear
    (
    void
    )
{
    set_all_segments(0xFF);

    set_digit_state(1, false);
    set_digit_state(2, false);

    segment_display.current_number = 0;
    segment_display.digit1_value = 0;
    segment_display.digit2_value = 0;
    
    return ERRCODE_SUCC;
}

void func_segment_display_process
    (
    uint32_t elapsed_ms
    )
{
    // 数码管动态扫描刷新
    segment_display.refresh_timer += elapsed_ms;
    
    // 每xms切换一次显示位
    if (segment_display.refresh_timer >= 8)
    {
        segment_display.refresh_timer = 0;
        
        if (segment_display.is_digit1_active)
        {
            // 显示十位
            set_all_segments(digit_to_segments[segment_display.digit1_value]);
            set_digit_state(1, true);
            segment_display.is_digit1_active = false;
        } 
        else
        {
            // 显示个位
            set_all_segments(digit_to_segments[segment_display.digit2_value]);
            set_digit_state(2, true);
            segment_display.is_digit1_active = true;
        }
    }
}
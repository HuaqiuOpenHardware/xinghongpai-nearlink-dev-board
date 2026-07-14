#include "key_demo.h"


// 最大支持的按键数量
#define MAX_KEYS 2

// 按键状态
typedef enum {
    KEY_STATE_RELEASED = 0,         // 释放状态
    KEY_STATE_PRESSED_DEBOUNCE,     // 按下消抖状态
    KEY_STATE_PRESSED,              // 按下状态
    KEY_STATE_RELEASED_DEBOUNCE,    // 释放消抖状态
} key_state_t;

// 按键控制块
typedef struct {
    key_config_t config;          // 按键配置
    key_state_t state;            // 当前状态
    key_event_t event;            // 当前事件
    uint32_t debounce_counter;    // 消抖计数器
    bool last_level;              // 上一次的电平状态
    bool has_event;               // 是否有事件发生
} key_ctrl_t;

// 按键控制块数组
static key_ctrl_t keys[MAX_KEYS];
static uint8_t key_count = 0;

errcode_t func_key_init
    (
    void
    ) 
{
    memset(keys, 0, sizeof(keys));
    key_count = 0;
    return ERRCODE_SUCC;
}

errcode_t func_key_register(const key_config_t *config) 
{
    if (key_count >= MAX_KEYS)
    {
        return ERRCODE_FAIL; // 超出最大支持数量
    }

    IoTGpioInit(config->pin);
    
    // 初始化GPIO
    if (config->pin == GPIO_04)
    {
        uapi_pin_set_mode(config->pin, PIN_MODE_2);
    }
    else if (config->pin == GPIO_05)
    {
        uapi_pin_set_mode(config->pin, PIN_MODE_4);
    }
    else 
    {
        uapi_pin_set_mode(config->pin, HAL_PIO_FUNC_GPIO);
    }
    errcode_t ret = IoTGpioSetDir(config->pin, GPIO_DIRECTION_INPUT);
    if (ret != ERRCODE_SUCC)
    {
        return ret;
    }
    
    // 保存配置
    keys[key_count].config = *config;
    keys[key_count].state = KEY_STATE_RELEASED;
    keys[key_count].event = KEY_EVENT_NONE;
    keys[key_count].debounce_counter = 0;
    keys[key_count].last_level = (uapi_gpio_get_val(config->pin) == config->active_level);
    keys[key_count].has_event = false;
    
    key_count++;
    
    return ERRCODE_SUCC;
}

void func_key_process
    (
    uint32_t elapsed_ms
    )
{
    for (int i = 0; i < key_count; i++) 
    {
        key_ctrl_t *key = &keys[i];
        bool current_level = (uapi_gpio_get_val(key->config.pin) == key->config.active_level);
        
        switch (key->state) 
        {
            case KEY_STATE_RELEASED:
                if (current_level && !key->last_level)
                {
                    // 检测到按下，进入消抖状态
                    key->state = KEY_STATE_PRESSED_DEBOUNCE;
                    key->debounce_counter = 0;
                }
                break;
                
            case KEY_STATE_PRESSED_DEBOUNCE:
                key->debounce_counter += elapsed_ms;
                if (key->debounce_counter >= key->config.debounce_time)
                {
                    if (current_level)
                    {
                        // 确认按下
                        key->state = KEY_STATE_PRESSED;
                        key->event = KEY_EVENT_PRESS;
                        key->has_event = true;
                    } 
                    else 
                    {
                        // 回到释放状态
                        key->state = KEY_STATE_RELEASED;
                    }
                }
                break;
                
            case KEY_STATE_PRESSED:
                if (!current_level && key->last_level)
                {
                    // 检测到释放，进入消抖状态
                    key->state = KEY_STATE_RELEASED_DEBOUNCE;
                    key->debounce_counter = 0;
                }
                break;

            case KEY_STATE_RELEASED_DEBOUNCE:
                key->debounce_counter += elapsed_ms;
                if (key->debounce_counter >= key->config.debounce_time)
                {
                    if (!current_level)
                    {
                        // 确认释放
                        key->state = KEY_STATE_RELEASED;
                        key->event = KEY_EVENT_RELEASE;
                        key->has_event = true;
                    } 
                    else 
                    {
                        // 回到释放状态
                        key->state = KEY_STATE_PRESSED;
                    }
                }
                break;
        }
        
        key->last_level = current_level;
    }
}

key_event_t func_key_get_event
    (
    pin_t pin
    )
{
    for (int i = 0; i < key_count; i++)
    {
        if (keys[i].config.pin == pin && keys[i].has_event)
        {
            keys[i].has_event = false;
            return keys[i].event;
        }
    }
    return KEY_EVENT_NONE;
}

void func_key_clear_event
    (
    pin_t pin
    ) 
{
    for (int i = 0; i < key_count; i++) 
    {
        if (keys[i].config.pin == pin) 
        {
            keys[i].has_event = false;
            keys[i].event = KEY_EVENT_NONE;
            break;
        }
    }
}
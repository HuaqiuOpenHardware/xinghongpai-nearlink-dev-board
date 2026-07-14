#include "relay_demo.h"  // 假设存在继电器相关头文件


void func_relay_init
    (
    void
    )
{
    IoTGpioInit(GPIO_04);
    IoTGpioInit(GPIO_05);

    if (PIN_RELAY == GPIO_04)
    {
        uapi_pin_set_mode(PIN_RELAY, PIN_MODE_2);  // 设置引脚为GPIO模式
    }
    else if (PIN_RELAY == GPIO_05)
    {
        uapi_pin_set_mode(PIN_RELAY, PIN_MODE_4);  // 设置引脚为GPIO模式
    }

    // 初始化继电器控制引脚：设置为GPIO功能、输出方向、初始关闭状态
    uapi_pin_set_mode(PIN_RELAY, HAL_PIO_FUNC_GPIO);  // 设置引脚为GPIO模式
    IoTGpioSetDir(PIN_RELAY, GPIO_DIRECTION_OUTPUT);  // 设置为输出方向
}

void func_relay_on
    (
    void
    )
{
    // 继电器打开：设置控制引脚为高电平（根据硬件定义，此处假设高电平为打开）
    IoTGpioSetOutputVal(PIN_RELAY, GPIO_LEVEL_HIGH);
}

void func_relay_off
    (
    void
    )
{
    // 继电器关闭：设置控制引脚为低电平（与打开状态电平相反）
    IoTGpioSetOutputVal(PIN_RELAY, GPIO_LEVEL_LOW);
}
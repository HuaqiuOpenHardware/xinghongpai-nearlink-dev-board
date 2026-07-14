#include "fan_demo.h"
#include "iot_gpio.h"

void func_fan_init(void)
{
    IoTGpioInit(PIN_FORWARD);
    IoTGpioInit(PIN_BACKWARD);

    //用旧的接口，设置为普通GPIO
    uapi_pin_set_mode(PIN_FORWARD, HAL_PIO_FUNC_GPIO);

    IoTGpioSetDir(PIN_FORWARD, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(PIN_FORWARD, 0);

    uapi_pin_set_mode(PIN_BACKWARD, PIN_MODE_4);
    IoTGpioSetDir(PIN_BACKWARD, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(PIN_BACKWARD, 0);
}

void func_fan_forward(void)
{
    IoTGpioSetOutputVal(PIN_FORWARD, 1);
    IoTGpioSetOutputVal(PIN_BACKWARD, 0);
}

void func_fan_backward(void)
{
    IoTGpioSetOutputVal(PIN_FORWARD, 0);
    IoTGpioSetOutputVal(PIN_BACKWARD, 1);
}
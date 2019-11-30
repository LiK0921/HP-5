#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

#include "imx6ul.h"

typedef enum _gpio_pin_direction
{
    kGPIO_DigitalInput  = 0U,//输入
    kGPIO_DigitalOutput = 1U //输出
}gpio_pin_direction_t;

typedef struct _gpio_pin_config
{
    gpio_pin_direction_t direction;
    u8 outputLogic;
}gpio_pin_config_t;



void gpio_init(GPIO_Type * base, int pin, gpio_pin_config_t * config);

int gpio_read(GPIO_Type * base, int pin);

void gpio_write(GPIO_Type * base, int pin, int value);

#endif // !__BSP_GPIO_H
#include "bsp_gpio.h"

void gpio_init(GPIO_Type * base, int pin, gpio_pin_config_t * config)
{
    if (config->direction == kGPIO_DigitalInput)
    {
        base->GDIR &= ~(1 << pin); 
    }
    else
    {
        base->GDIR |= (1 << pin); 
    }
}

int gpio_read(GPIO_Type * base, int pin)
{
    return (base->DR >> pin) & 0x1;
}

void gpio_write(GPIO_Type * base, int pin, int value)
{
    if (value == 0U)
    {
        base->DR &= ~(1U << pin); /* 输出低电平 */
    }
    else
    {
        base->DR |= (1U << pin); /* 输出高电平 */
    }
}


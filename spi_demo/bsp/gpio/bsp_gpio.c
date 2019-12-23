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
        gpio_write(base, pin, config->outputLogic);	/* 设置默认输出电平 */
    }

    gpio_int_config(base, pin, config);
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

//配置GPIO的中断触发方式
void gpio_int_config(GPIO_Type * base, int pin, gpio_pin_config_t * config)
{
    volatile u32 * icr;
    u32 icrShift = pin;
    //禁止双边缘触发
    base->EDGE_SEL &= ~(1 << pin);

    if (pin < 16)
    {
        icr = &(base->ICR1);//低16个pin
    }
    else
    {
        icr = &(base->ICR2);//高16个pin
        icrShift -= 16;
    }
    //开始设置触发方式，2 Bits控制一个pin
    switch (config->interrupt_mode)
    {
        case kGPIO_IntLowlevel:
            *icr &= ~(3 << (icrShift * 2));
            break;
        case kGPIO_IntHighlevel:
            *icr &= ~(3 << (icrShift * 2));
            *icr |= (1 << (icrShift * 2));
            break;
        case kGPIO_IntRisingEdge:
            *icr &= ~(3 << (icrShift * 2));
            *icr |= (2 << (icrShift * 2));
            break;
        case kGPIO_IntFallingEdge:
            *icr &= ~(3 << (icrShift * 2));
            *icr |= (3 << (icrShift * 2));
            break;
        case kGPIO_IntRisingOrFallingEdge:
            base->EDGE_SEL |= (1 << pin);
            break;
        
        default:
            break;
    }
}
//中断使能
void gpio_int_enble(GPIO_Type * base, int pin)
{
    base->IMR |= (1 << pin);//使能中断
}
//失能中断
void gpio_int_disenble(GPIO_Type * base, int pin)
{
    base->IMR &= ~(1 << pin);//失能中断
}
//清除中断标志位
void gpio_int_clearflag(GPIO_Type * base, int pin)
{
    base->ISR |= (1 << pin);//写１清除中断标志位
}
#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

#include "imx6ul.h"

typedef enum _gpio_pin_direction
{
    kGPIO_DigitalInput  = 0U,//输入
    kGPIO_DigitalOutput = 1U //输出
}gpio_pin_direction_t;

typedef enum _gpio_interrupt_mode
{
    kGPIO_NoIntmode = 0U,/* 无中断功能 */
    kGPIO_IntLowlevel = 1U,/* 低电平触发	*/
    kGPIO_IntHighlevel = 2U,/* 高电平触发 */
    kGPIO_IntRisingEdge = 3U, 			/* 上升沿触发	*/
    kGPIO_IntFallingEdge = 4U, 			/* 下降沿触发 */
    kGPIO_IntRisingOrFallingEdge = 5U, 	/* 上升沿和下降沿都触发 */

}gpio_interrupt_mode_t;

typedef struct _gpio_pin_config
{
    gpio_pin_direction_t direction;
    u8 outputLogic;
    gpio_interrupt_mode_t interrupt_mode;
}gpio_pin_config_t;



void gpio_init(GPIO_Type * base, int pin, gpio_pin_config_t * config);

int gpio_read(GPIO_Type * base, int pin);

void gpio_write(GPIO_Type * base, int pin, int value);

void gpio_int_config(GPIO_Type * base, int pin, gpio_pin_config_t * config);
void gpio_int_enble(GPIO_Type * base, int pin);
void gpio_int_disenble(GPIO_Type * base, int pin);
void gpio_int_clearflag(GPIO_Type * base, int pin);
#endif // !__BSP_GPIO_H
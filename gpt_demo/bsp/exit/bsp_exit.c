#include "bsp_exit.h"
#include "bsp_gpio.h"
#include "bsp_int.h"
#include "bsp_delay.h"
#include "bsp_beep.h"
#include "imx6ul.h"
#include "bsp_key.h"
#include "bsp_epit.h"
void exit_init(void)
{
    key_init();
    
    //使能指定的中断
    GIC_EnableIRQ(GPIO1_Combined_16_31_IRQn);
    //将按键对应的gpio脚注册进去
    sys_register_irqhandle(GPIO1_Combined_16_31_IRQn, (system_irq_handler_t)gpio1_16_31_irqhandle, NULL);
    //开启中断
    gpio_int_enble(GPIO1, 18);
}
//按键的中断服务函数，翻转蜂鸣器
void gpio1_16_31_irqhandle(void)
{
    //重开定时器
    epit1_restart(66000000 / 100);
    //清除中断标记位
    gpio_int_clearflag(GPIO1, 18);
}



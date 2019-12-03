#include "bsp_epit.h"
#include "bsp_int.h"
#include "bsp_led.h"
void epit1_init(u32 frac, u32 loadvalue)
{
    //分频系数限幅
    if (frac > 4095)
    {
        frac = 4095;
    }
    EPIT1->CR = 0;//先清零

    EPIT1->CR = (1 << 24) | (frac << 4) | (1 << 3) | (1 << 2)| (1 << 1);
    //重装载值
    EPIT1->LR = loadvalue;
    //比较寄存器设置为零
    EPIT1->CMPR = 0;
    //使能定时器中断
    GIC_EnableIRQ(EPIT1_IRQn);
    //注册定时器中断服务函数
    sys_register_irqhandle(EPIT1_IRQn, (system_irq_handler_t)epit1_irqhandle, NULL);

    //开启计时
    EPIT1->CR |= (1 << 0);
}

void epit1_irqhandle(void)
{
    static u8 state = 0;
    state = !state;
    if (EPIT1->SR & (1 << 0))
    {
        led_switch(LED0, state);
    }
    EPIT1->SR |= (1 << 0);//写１清除中断标志位
}


#include "bsp_epit.h"
#include "bsp_int.h"
#include "bsp_beep.h"
#include "bsp_gpio.h"
#include "bsp_lcd.h"
void epit_init(EPIT_Type * base, u32 frac, u32 loadvalue)
{
    //分频系数限幅
    if (frac > 4095)
    {
        frac = 4095;
    }
    base->CR = 0;//先清零

    base->CR = (1 << 24) | (frac << 4) | (1 << 3) | (1 << 2)| (1 << 1);
    //重装载值
    base->LR = loadvalue;
    //比较寄存器设置为零
    base->CMPR = 0;

 #ifdef EPIT1_ENABLE
    //注册定时器中断服务函数
    sys_register_irqhandle(EPIT1_IRQn, (system_irq_handler_t)epit1_irqhandle, NULL);
    //使能定时器中断
    GIC_EnableIRQ(EPIT1_IRQn);
 #endif // EPIT1_ENABLE  
    

    //开启计时
    base->CR |= (1 << 0);
}

void epit_stop(EPIT_Type * base)
{
    base->CR &= ~(1 << 0);
}
void epit_restart(EPIT_Type * base, u32 loadvalue)
{
    base->CR &= ~(1 << 0);
    base->LR = loadvalue;
    //开启计时
    base->CR |= (1 << 0);
}


#ifdef EPIT1_ENABLE
void epit1_irqhandle(u32 giccIar, void * user_param)
{
    static u8 state = OFF;
    
    if (EPIT1->SR & (1 << 0))//10ms到了
    {
        epit_stop(EPIT1);//关闭定时器，等待下一次按下开始计时
        if (gpio_read(GPIO1, 18) == 0)//案件还按下了
        {
            state = !state;
            beep_switch(state);

            //改变屏幕背光亮度，改变占空比
            tft_lcd_struct.backlight_pwm += 10;
            if (tft_lcd_struct.backlight_pwm > 100)
            {
                tft_lcd_struct.backlight_pwm = 0;
            }
            
        }   
    }
    EPIT1->SR |= (1 << 0);//写１清除中断标志位
}
#endif // EPIT1_ENABLE  

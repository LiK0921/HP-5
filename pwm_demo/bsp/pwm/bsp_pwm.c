#include "bsp_pwm.h"
#include "bsp_int.h"
#include "bsp_lcd.h"

void pwm_init(PWM_Type * base, u32 hz)
{
    u8 i = 0;

    //控制寄存器清零
    base->PWMCR = 0;
                    //FIFO空余大于等于２时中断发生，时钟源IPG_CLK 66MHz,
    base->PWMCR |= (1 << 26) | (1 << 16) | (65 << 4);//66分频
    //以上设置了PWM1的计数频率为1MHz,即1us计数器加1
    
    //设置周期
    pwm_set_period(base, hz);

    if (base == PWM1)
    {
#if PWM1_ENABLE
    //初始化为复用pwm1
    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO08_PWM1_OUT, 0);
    //设置电气属性
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO08_PWM1_OUT, 0xb090);

    for ( i = 0; i < 4; i++)
    {     
        pwm_set_duty(PWM1, tft_lcd_struct.backlight_pwm);
    }
    
    //将PWM1中断注册进去
    sys_register_irqhandle(PWM1_IRQn, (system_irq_handler_t)pwm1_irqhandle, NULL);
    //使能指定的中断
    GIC_EnableIRQ(PWM1_IRQn);//一定要在注册函数的下面！！否则程序就卡死！！当我没说～
    
#endif
    }
    
    //FIFO为空时将产生中断
    pwm_fifo_int_enable(base);
    //一般将状态寄存器清零,写1清零
    base->PWMSR = 0xff;
    //使能PWM输出
    pwm_enable(base);
}

//u8 test_count = 0;

#if PWM1_ENABLE   
//内部不可加而外的东西，否则不执行中断服务函数！！！当我没说～，可能不能加printf()这种阻塞型函数
void pwm1_irqhandle(u32 giccIar, void * user_param)
{
    if (PWM1->PWMSR & (1 << 3))//FIFO低于"水位线"
    {  
        //test_count++;
        pwm_set_duty(PWM1, tft_lcd_struct.backlight_pwm);
        PWM1->PWMSR |= (1 << 3);//写1清除中断标志位
    }
}
#endif

void pwm_enable(PWM_Type * base)
{
    base->PWMCR |= (1 << 0);
}

void pwm_disable(PWM_Type * base)
{
    base->PWMCR &= ~(1 << 0);
}
void pwm_fifo_int_enable(PWM_Type * base)
{
    base->PWMIR |= 1 << 0;
}


void pwm_set_period(PWM_Type * base, u32 hz)
{
    if (hz < 2)
    {
        hz = 2;
    }
    else
    {
        hz -= 2;
    }
    
    base->PWMPR = hz & 0xffff;//period register,      bit[15:0]
}

//设置占空比，duty=0~100,也就是100份
void pwm_set_duty(PWM_Type * base, u8 duty)
{
    u16 period = base->PWMPR + 2;

    u16 sample_value = (u16)(period * duty / 100.0f);

    //sample register,bit[15:0]
    base->PWMSAR = sample_value & 0xffff;//设置采样值，就是翻转电平的计数值
}
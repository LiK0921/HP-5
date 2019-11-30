/*
    c语言点灯开始
 */
#include "main.h"

void clk_enble(void)
{
    CCM->CCGR0 = 0xffffffff;
    CCM->CCGR1 = 0xffffffff;
    CCM->CCGR2 = 0xffffffff;
    CCM->CCGR3 = 0xffffffff;
    CCM->CCGR4 = 0xffffffff;
    CCM->CCGR5 = 0xffffffff;
    CCM->CCGR6 = 0xffffffff;
}

void led_init(void)
{
    IOMUX_SW_MUX->GPIO1_IO03 = 0x05;

    IOMUX_SW_PAD->GPIO1_IO03 = 0x10b0;

    GPIO1->GDIR = 0x0000008;

    GPIO1->DR = 0x0;
}

void led_on(void)
{
    GPIO1->DR &= ~(1 << 3);
}

void led_off(void)
{
    GPIO1->DR |= (1 << 3);
}

void delay_short(volatile unsigned int n)
{
    while (n--)
    {

    }
}

void delay(volatile unsigned int n)
{
    while (n--)
    {
        delay_short(0x7ff);
    }
    
}

int main(void) 
{
    clk_enble();

    led_init();

    while (1)
    {     
        led_on();
        delay(200);
        led_off();
        delay(200);
    }  
    return 0;
}





/*
    c语言点灯开始
 */
#include "MCIMX6Y2.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"

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
    //初始化为复用GPIO
    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO03_GPIO1_IO03, 0);
    //设置电气属性
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO03_GPIO1_IO03, 0x10b0);
    //设置方向为输出
    GPIO1->GDIR = 0x0000008;
    //从这组GPIO的所有IO，00~31，都写入低电平
    GPIO1->DR = 0x0;
}

void led_on(void)
{
    /* 将 GPIO1_DR 的 bit3 清零 */
    GPIO1->DR &= ~(1 << 3);
}

void led_off(void)
{
    /* 将 GPIO1_DR 的 bit3 置 1 */
    GPIO1->DR |= (1 << 3);
}

/*
* @description : 短时间延时函数
* @param - n : 要延时循环次数(空操作循环次数，模式延时)
* @return : 无
*/
void delay_short(volatile unsigned int n)
{
    while (n--)
    {
    }
}
/*
* @description : 延时函数,在 396Mhz 的主频下
* 延时时间大约为 1ms
* @param - n : 要延时的 ms 数
* @return : 无
*/
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
        delay(100);
        led_off();
        delay(100);
    }  
    return 0;
}





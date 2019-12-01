#include "bsp_key.h"
#include "bsp_gpio.h"
#include "bsp_delay.h"
void key_init(void)
{
    gpio_pin_config_t gpio_config;
    gpio_config.direction = kGPIO_DigitalInput;
   
    /* 1、初始化IO复用，复用为GPIO5_IO01 */
    IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18, 0);
    //设置电气属性
    /* 2、、配置GPIO1_IO03的IO属性	
	 *bit 16:0 HYS关闭
	 *bit [15:14]: 00 默认下拉
	 *bit [13]: 0 kepper功能
	 *bit [12]: 1 pull/keeper使能
	 *bit [11]: 0 关闭开路输出
	 *bit [7:6]: 10 速度100Mhz
	 *bit [5:3]: 110 R0/6驱动能力
	 *bit [0]: 0 低转换率
	 */
    IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18, 0xf080);//上拉，关闭输出

    gpio_init(GPIO1, 18, &gpio_config);
}
/*
 * @description	: 获取按键值 
 * @param 		: 无
 * @return 		: 0 没有按键按下，其他值:对应的按键值
 */
int key_getvalue(void)
{
    int ret = 0;
    static unsigned char release = 1; /* 按键松开 */
    if (release == 1 && gpio_read(GPIO1, 18) == 0)
    {
        delay(10);
        release = 0;//按键按下
        if (gpio_read(GPIO1, 18) == 0)
        {
            ret = KEY0_VALUE;
        }
    }
    else if (gpio_read(GPIO1, 18) == 1)
    {
        ret = KEY_NONE;
        release = 1;
    }
    return ret;
}

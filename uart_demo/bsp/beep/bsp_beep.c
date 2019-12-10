#include "bsp_beep.h"
#include "bsp_gpio.h"
#include "cc.h"

void beep_init(void)
{
    gpio_pin_config_t gpio_config;
    gpio_config.direction = kGPIO_DigitalOutput;
    gpio_config.outputLogic = 1;
   
    /* 1、初始化IO复用，复用为GPIO5_IO01 */
    IOMUXC_SetPinMux(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01, 0);
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
    IOMUXC_SetPinConfig(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01, 0x10b0);

    gpio_init(GPIO5, 1, &gpio_config);
}

void beep_switch(int status)
{
    if (status == ON)
    {
        gpio_write(GPIO5, 1, 0);
    }
    else if (status == OFF)
    {
        gpio_write(GPIO5, 1, 1);
    } 
}
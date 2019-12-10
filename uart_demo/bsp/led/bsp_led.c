#include "bsp_led.h"
#include "cc.h"
#include "bsp_gpio.h"

void led_on(void)
{
    gpio_write(GPIO1, 3, 0);
    /* 将 GPIO1_DR 的 bit3 清零 */
    //GPIO1->DR &= ~(1 << 3);
}

void led_off(void)
{
    gpio_write(GPIO1, 3, 1);
    /* 将 GPIO1_DR 的 bit3 置 1 */
    //GPIO1->DR |= (1 << 3);
}

void led_init(void)
{
    gpio_pin_config_t gpio_config;
    gpio_config.direction = kGPIO_DigitalOutput;
    gpio_config.outputLogic = 1;
    //初始化为复用GPIO
    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO03_GPIO1_IO03, 0);
    //设置电气属性
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO03_GPIO1_IO03, 0x10b0);

    gpio_init(GPIO1, 3, &gpio_config);

    //设置方向为输出
    //GPIO1->GDIR = 0x0000008;
    //从这组GPIO的所有IO，00~31，都写入低电平
    //GPIO1->DR = 0x0;
}

void led_switch(int led, int status)
{
    switch (led)
    {
    case LED0:
        if (status == ON)
        {

            led_on();
        }
        else
        {
            led_off();
        } 
        break;
    
    default:
        break;
    }
}


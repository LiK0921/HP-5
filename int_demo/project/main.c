#include "imx6ul.h"

#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_beep.h"
#include "bsp_exit.h"
#include "bsp_int.h"

int main(void) 
{
    u8 led_state = OFF;

    //中断初始化
    int_init();
    //芯片时钟树配置到推荐频率
    im6ull_clk_init();
    //打开所有外设的时钟
    clk_enble();
    //LED初始化
    led_init();
    //蜂鸣器初始化
    beep_init();
    //按键中断初始化
    exit_init();		

    while (1)
    {     
        led_state = !led_state;
		led_switch(LED0, led_state);
		delay(100);   
    }  
    return 0;
}





#include "imx6ul.h"

#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_beep.h"
#include "bsp_exit.h"
#include "bsp_int.h"
#include "bsp_epit.h"
int main(void) 
{
    u8 state = OFF;
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
    epit1_init(0, 66000000 / 100);	
  
    while (1)
    {     
		  delay(100); 
      state = !state;
      led_switch(LED0, state);  
    }  
    return 0;
}





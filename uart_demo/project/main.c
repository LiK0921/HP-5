#include "imx6ul.h"
#include "bsp_uart.h"
#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_beep.h"
#include "bsp_exit.h"
#include "bsp_int.h"
#include "bsp_epit.h"
#include "bsp_gpt.h"
int main(void) 
{
    u8 c;
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
    epit1_init(0, 66000000 / 100);//1秒分成100份，就是10ms溢出	

    gpt_delay_init();//高精度延时

    uart_init(UART1, 115200);
    while (1)
    {
        my_puts(UART1, "Please Input A Char : ");
        c = my_getc(UART1);
        //my_putc(UART1, c);
        my_puts(UART1, "\r\n");

        my_puts(UART1, "Your Input Is : ");
        my_putc(UART1, c);
        my_puts(UART1, "\r\n");
    }
    return 0;
}





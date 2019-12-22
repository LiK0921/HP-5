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
#include "stdio.h"
#include "bsp_lcd.h"
#include "bsp_lcdapi.h"
#include "bsp_rtc.h"


//背景色
u32 backclor[10] = {
    LCD_BLUE, LCD_GREEN, LCD_RED, LCD_CYAN, LCD_YELLOW,
    LCD_LIGHTBLUE, LCD_DARKBLUE, LCD_WHITE, LCD_BLACK,
    LCD_ORANGE
};

int main(void) 
{
    RTC_Struct rtc_structure;
    char buf[160]; 
    memset(buf, 0, sizeof(buf));

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
    //定时器初始化
    epit1_init(0, 66000000 / 100);//1秒分成100份，就是10ms溢出	
    
    //定时器初始化
    gpt_delay_init();//高精度延时
    //串口１初始化
    uart_init(UART1, 115200);
    //lcd初始化
    lcd_init();
    //rtc初始化
    rtc_init();
   
    
    tft_lcd_struct.forecolor = LCD_BLACK;
    tft_lcd_struct.backcolor = LCD_ORANGE;
    lcd_clear(LCD_ORANGE);//刷新背景色

    rtc_structure.year = 2019;
    rtc_structure.month = 12;
    rtc_structure.day = 22;
    rtc_structure.hour = 10;
    rtc_structure.minute = 47;
    rtc_structure.second = 0;
    //设置时间
    //rtc_setdatetime(&rtc_structure);

    lcd_show_string(50, 10, 260, 32, 32,(char*)"RTC DEMO~");
    while (1)
    {

        rtc_getdatetime(&rtc_structure);

        sprintf(buf, "%4d/%2d/%2d %2d:%2d:%2d",rtc_structure.year, rtc_structure.month, rtc_structure.day,
                                        rtc_structure.hour, rtc_structure.minute, rtc_structure.second);
        lcd_show_string(50, 70, 450, 32, 32,(char*)buf);

        gpt_delay_ms(1000);
       	
    }
    return 0;
}








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
//背景色
u32 backclor[10] = {
    LCD_BLUE, LCD_GREEN, LCD_RED, LCD_CYAN, LCD_YELLOW,
    LCD_LIGHTBLUE, LCD_DARKBLUE, LCD_WHITE, LCD_BLACK,
    LCD_ORANGE
};

int main(void) 
{
    //s32 a, b;
    u32 color = 0, index = 0;
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

    //画点
    lcd_drawpoint(0, 0, LCD_BLUE);
    lcd_drawpoint(tft_lcd_struct.width -1, 0, LCD_BLUE);
    lcd_drawpoint(0, tft_lcd_struct.height - 1, LCD_BLUE);
    lcd_drawpoint(tft_lcd_struct.width -1, tft_lcd_struct.height - 1, LCD_BLUE);
    
    //读取０，０点的颜色值
    color = lcd_readpoint(0, 0);
    printf("(0, 0) color is %x\n", color);//输出颜色值

    tft_lcd_struct.forecolor = LCD_RED;
    tft_lcd_struct.backcolor = LCD_WHITE;
    while (1)
    {
        index++;
        lcd_clear(backclor[index]);//刷新背景色
        if (index == 10)
            index = 0;

        //改变字体背景色
        tft_lcd_struct.backcolor = backclor[index];

        lcd_show_string(10,10, 255, 32, 32,(char *)"HelloWorld");
        lcd_show_string(400,280, 255, 32, 32,(char *)"Kao Yan Jia You!!!");
        
        gpt_delay_ms(500);
        //delay(500);
		//scanf("%d %d", &a, &b);		
    }
    return 0;
}





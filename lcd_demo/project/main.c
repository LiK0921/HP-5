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
    //LED初始化backclor[index]
    beep_init();
    //按键中断初始化
    exit_init();
    epit1_init(0, 66000000 / 100);//1秒分成100份，就是10ms溢出	

    gpt_delay_init();//高精度延时

    uart_init(UART1, 115200);

    lcd_init();

    lcd_drawpoint(0, 0, LCD_BLUE);
    lcd_drawpoint(tft_lcd_struct.width -1, 0, LCD_BLUE);
    lcd_drawpoint(0, tft_lcd_struct.height - 1, LCD_BLUE);
    lcd_drawpoint(tft_lcd_struct.width -1, tft_lcd_struct.height - 1, LCD_BLUE);
    color = lcd_readpoint(0, 0);
    printf("(0, 0) color is %x\n", color);

    tft_lcd_struct.forecolor = LCD_RED;
    tft_lcd_struct.backcolor = LCD_WHITE;
    while (1)
    {
        index++;
        lcd_clear(backclor[index]);
        if (index == 10)
            index = 0;

        tft_lcd_struct.backcolor = backclor[index];
        lcd_show_string(10,10, 255, 32, 32,(char *)"HelloWorld");
        lcd_show_string(400,280, 255, 32, 32,(char *)"Kao Yan Jia You!!!");
        delay(500);
		//scanf("%d %d", &a, &b);		
    }
    return 0;
}





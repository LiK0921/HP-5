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
#include "bsp_ap3216c.h"
#include "bsp_icm20608.h"

//背景色
u32 backclor[10] = {
    LCD_BLUE, LCD_GREEN, LCD_RED, LCD_CYAN, LCD_YELLOW,
    LCD_LIGHTBLUE, LCD_DARKBLUE, LCD_WHITE, LCD_BLACK,
    LCD_ORANGE
};

int main(void) 
{
    u16 ir, ps, als;
    RTC_Struct rtc_structure;
    char buf[160]; 
    memset(buf, 0, sizeof(buf));

    imx6ul_hardfpu_enable(); /* 开启硬件浮点运算及ENON */
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
    //ap3216c初始化，i2c器件
    ap3216c_init();
    //icm20608初始化
    icm20608_init();
   
    
    tft_lcd_struct.forecolor = LCD_BLACK;
    tft_lcd_struct.backcolor = LCD_ORANGE;
    lcd_clear(LCD_ORANGE);//刷新背景色
/*
    rtc_structure.year = 2019;
    rtc_structure.month = 12;
    rtc_structure.day = 22;
    rtc_structure.hour = 22;
    rtc_structure.minute = 37;
    rtc_structure.second = 0;
    //设置时间
    //rtc_setdatetime(&rtc_structure);
*/
    //iic//////////////////////////////////////////////////////////////////////////////////////////////
    lcd_show_string(20, 10, 260, 32, 32,(char*)"IIC ADN SPI DEMO~");
    lcd_show_string(20, 100, 70, 24, 24,(char*)"IR :");
    lcd_show_string(20, 130, 70, 24, 24,(char*)"PS :");
    lcd_show_string(20, 160, 70, 24, 24,(char*)"ALS:");
    //spi//////////////////////////////////////////////////////////////////////////////////////////////
    lcd_show_string(50, 230, 200, 16, 16, (char*)"accel x:");  
	lcd_show_string(50, 250, 200, 16, 16, (char*)"accel y:");  
	lcd_show_string(50, 270, 200, 16, 16, (char*)"accel z:");  
	lcd_show_string(50, 290, 200, 16, 16, (char*)"gyro  x:"); 
	lcd_show_string(50, 310, 200, 16, 16, (char*)"gyro  y:"); 
	lcd_show_string(50, 330, 200, 16, 16, (char*)"gyro  z:"); 
	lcd_show_string(50, 350, 200, 16, 16, (char*)"temp   :"); 

	lcd_show_string(50 + 181, 230, 200, 16, 16, (char*)"g");  
	lcd_show_string(50 + 181, 250, 200, 16, 16, (char*)"g");  
	lcd_show_string(50 + 181, 270, 200, 16, 16, (char*)"g");  
	lcd_show_string(50 + 181, 290, 200, 16, 16, (char*)"o/s"); 
	lcd_show_string(50 + 181, 310, 200, 16, 16, (char*)"o/s"); 
	lcd_show_string(50 + 181, 330, 200, 16, 16, (char*)"o/s"); 
	lcd_show_string(50 + 181, 350, 200, 16, 16, (char*)"C");

    while (1)
    {
        //////////////////////////////////////////////////////////////////////////////////////////////
        ap3216c_read_data(&ir, &ps, &als);
        lcd_shownum(100, 100, ir, 5, 24);
        lcd_shownum(100, 130, ps, 5, 24);
        lcd_shownum(100, 160, als, 5, 24);
        //////////////////////////////////////////////////////////////////////////////////////////////
        rtc_getdatetime(&rtc_structure);
        sprintf(buf, "%4d/%2d/%2d %2d:%2d:%2d",rtc_structure.year, rtc_structure.month, rtc_structure.day,
                                        rtc_structure.hour, rtc_structure.minute, rtc_structure.second);
        lcd_show_string(20, 70, 450, 32, 32,(char*)buf);
        //////////////////////////////////////////////////////////////////////////////////////////////
        icm20608_getdata();

        integer_display(50 + 70, 230, 16, icm20608_structure.accel_x_adc);
		integer_display(50 + 70, 250, 16, icm20608_structure.accel_y_adc);
		integer_display(50 + 70, 270, 16, icm20608_structure.accel_z_adc);
		integer_display(50 + 70, 290, 16, icm20608_structure.gyro_x_adc);
		integer_display(50 + 70, 310, 16, icm20608_structure.gyro_y_adc);
		integer_display(50 + 70, 330, 16, icm20608_structure.gyro_z_adc);
		integer_display(50 + 70, 350, 16, icm20608_structure.temp_adc);

		decimals_display(50 + 70 + 50, 230, 16, icm20608_structure.accel_x_act);
		decimals_display(50 + 70 + 50, 250, 16, icm20608_structure.accel_y_act);
		decimals_display(50 + 70 + 50, 270, 16, icm20608_structure.accel_z_act);
		decimals_display(50 + 70 + 50, 290, 16, icm20608_structure.gyro_x_act);
		decimals_display(50 + 70 + 50, 310, 16, icm20608_structure.gyro_y_act);
		decimals_display(50 + 70 + 50, 330, 16, icm20608_structure.gyro_z_act);
		decimals_display(50 + 70 + 50, 350, 16, icm20608_structure.temp_act);
        //////////////////////////////////////////////////////////////////////////////////////////////
        gpt_delay_ms(110);
    }
    return 0;
}








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
#include "bsp_ft5426.h"

//背景色
u32 backclor[10] = {
    LCD_BLUE, LCD_GREEN, LCD_RED, LCD_CYAN, LCD_YELLOW,
    LCD_LIGHTBLUE, LCD_DARKBLUE, LCD_WHITE, LCD_BLACK,
    LCD_ORANGE
};

int main(void) 
{
    u8 i = 0, j = 0;
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
    //ft5426触摸屏初始化
    ft5426_init();
   
    
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
    //iic ap3216cs//////////////////////////////////////////////////////////////////////////////////////////////
    lcd_show_string(20, 10, 260, 32, 32,(char*)"IIC TOUCH DEMO~");
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

    //iic touch//////////////////////////////////////////////////////////////////////////////////////////////
    lcd_show_string(500, 110, 400, 16, 16,	(char*)"TP Num	:");  
	lcd_show_string(500, 130, 200, 16, 16,	(char*)"Point0 X:");  
	lcd_show_string(500, 150, 200, 16, 16,	(char*)"Point0 Y:");  
	lcd_show_string(500, 170, 200, 16, 16,	(char*)"Point1 X:");  
	lcd_show_string(500, 190, 200, 16, 16,	(char*)"Point1 Y:");  
	lcd_show_string(500, 210, 200, 16, 16,	(char*)"Point2 X:");  
	lcd_show_string(500, 230, 200, 16, 16,	(char*)"Point2 Y:");  
	lcd_show_string(500, 250, 200, 16, 16,	(char*)"Point3 X:");  
	lcd_show_string(500, 270, 200, 16, 16,	(char*)"Point3 Y:");  
	lcd_show_string(500, 290, 200, 16, 16,	(char*)"Point4 X:");  
	lcd_show_string(500, 310, 200, 16, 16,	(char*)"Point4 Y:");  

    while (1)
    {
        i++, j++;
        i %= 3, j %= 20;
        //////////////////////////////////////////////////////////////////////////////////////////////
        if (i == 0)//150ms读取一次光强传感器的数据，因为iic传输比较慢
        {
            ap3216c_read_data(&ir, &ps, &als);
            lcd_shownum(100, 100, ir, 5, 24);//显示红外，接近距离，光强度
            lcd_shownum(100, 130, ps, 5, 24);
            lcd_shownum(100, 160, als, 5, 24);         
        }
        if (j == 0)//1000ms到
        {
            //////////////////////////////////////////////////////////////////////////////////////////////
            rtc_getdatetime(&rtc_structure);//获取时间，一秒获取一次
            sprintf(buf, "%4d/%2d/%2d %2d:%2d:%2d",rtc_structure.year, rtc_structure.month, rtc_structure.day,
                                            rtc_structure.hour, rtc_structure.minute, rtc_structure.second);
            lcd_show_string(20, 70, 450, 32, 32,(char*)buf);//显示时间
        }   
        //////////////////////////////////////////////////////////////////////////////////////////////
        icm20608_getdata();
        //显示ADC的值
        integer_display(50 + 70, 230, 16, icm20608_structure.accel_x_adc);
		integer_display(50 + 70, 250, 16, icm20608_structure.accel_y_adc);
		integer_display(50 + 70, 270, 16, icm20608_structure.accel_z_adc);
		integer_display(50 + 70, 290, 16, icm20608_structure.gyro_x_adc);
		integer_display(50 + 70, 310, 16, icm20608_structure.gyro_y_adc);
		integer_display(50 + 70, 330, 16, icm20608_structure.gyro_z_adc);
		integer_display(50 + 70, 350, 16, icm20608_structure.temp_adc);
        //显示量程转换的实际值
		decimals_display(50 + 70 + 50, 230, 16, icm20608_structure.accel_x_act);
		decimals_display(50 + 70 + 50, 250, 16, icm20608_structure.accel_y_act);
		decimals_display(50 + 70 + 50, 270, 16, icm20608_structure.accel_z_act);
		decimals_display(50 + 70 + 50, 290, 16, icm20608_structure.gyro_x_act);
		decimals_display(50 + 70 + 50, 310, 16, icm20608_structure.gyro_y_act);
		decimals_display(50 + 70 + 50, 330, 16, icm20608_structure.gyro_z_act);
		decimals_display(50 + 70 + 50, 350, 16, icm20608_structure.temp_act);
        //////////////////////////////////////////////////////////////////////////////////////////////
        lcd_shownum(500 + 72, 110, ft5426_struct.point_num , 1, 16);//显示触摸点的个数
		lcd_shownum(500 + 72, 130, ft5426_struct.x[0], 5, 16);
		lcd_shownum(500 + 72, 150, ft5426_struct.y[0], 5, 16);
		lcd_shownum(500 + 72, 170, ft5426_struct.x[1], 5, 16);
		lcd_shownum(500 + 72, 190, ft5426_struct.y[1], 5, 16);
		lcd_shownum(500 + 72, 210, ft5426_struct.x[2], 5, 16);
		lcd_shownum(500 + 72, 230, ft5426_struct.y[2], 5, 16);
		lcd_shownum(500 + 72, 250, ft5426_struct.x[3], 5, 16);
		lcd_shownum(500 + 72, 270, ft5426_struct.y[3], 5, 16);
		lcd_shownum(500 + 72, 290, ft5426_struct.x[4], 5, 16);
		lcd_shownum(500 + 72, 310, ft5426_struct.y[4], 5, 16);   

        //lcd_draw_bigpoint(ft5426_struct.x[0], ft5426_struct.y[0], LCD_BLUE);//画四个点跟着手指动         
        //////////////////////////////////////////////////////////////////////////////////////////////
        gpt_delay_ms(50);
    }
    return 0;
}








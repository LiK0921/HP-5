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
#include "bsp_pwm.h"
#include "bsp_adc.h"
#include "imu.h"
#include "Ano_Imu.h"
#include "bsp_filter.h"

//量程
extern float   gyroscale;
extern u16     accescale;

s32     Accel_Angle_s32[3] = { 0 };
float   Accel_Angle[3] = { 0 };

s32     Filter_Angle_s32[3] = { 0 };
float   Filter_Angle[3] = { 0 };

float Accel[3], Gyro[3];
float Pitch, Roll, Yaw; 

void my_func(void)
{
    Gyro[x] = (float)(icm20608_structure.gyro_x_adc / gyroscale / 57.30f);
    Gyro[y] = (float)(icm20608_structure.gyro_y_adc / gyroscale / 57.30f);
    Gyro[z] = (float)(icm20608_structure.gyro_z_adc / gyroscale / 57.30f);
    //Gyro[x] = (float)(icm20608_structure.gyro_x_adc / gyroscale);
    //Gyro[y] = (float)(icm20608_structure.gyro_y_adc / gyroscale);
    //Gyro[z] = (float)(icm20608_structure.gyro_z_adc / gyroscale);
    Accel[x] = (float)(icm20608_structure.accel_x_adc);
    Accel[y] = (float)(icm20608_structure.accel_y_adc);
    Accel[z] = (float)(icm20608_structure.accel_z_adc);

    //加速度直接比力出来的角度，噪声大，用于观测而已
    Accel_Angle[x] = fast_atan2(Accel[y], Accel[z]) * 180 / MY_PPPIII; //计算倾角	
    Accel_Angle[y] = fast_atan2(Accel[x], Accel[z]) * 180 / MY_PPPIII; //计算倾角	
    //滤波测试
    Yijie_Filter(Accel_Angle, Gyro, 0.005f);//两种滤波都要保证采样时间准确，否则时间不是严格的5ms则收敛很慢！
    //Kalman_Filter(Accel_Angle, Gyro, 0.005f);//芯片处理能力太low了！
    //显示观测角度
    Accel_Angle_s32[x] = Accel_Angle[x] * 100;
    Accel_Angle_s32[y] = Accel_Angle[y] * 100;
    //显示滤波角度
    Filter_Angle_s32[x] = Filter_Angle[x] * 100;
    Filter_Angle_s32[y] = Filter_Angle[y] * 100;

    /*update(0.005f, Gyro[x], Gyro[y], Gyro[z], Accel[x], Accel[y], Accel[z], &Roll, &Pitch, &Yaw);       
    Filter_Angle_s32[x] = (s32)(Roll * 100);
    Filter_Angle_s32[y] = (s32)(Pitch * 100);
    Filter_Angle_s32[z] = (s32)(Yaw * 100);*/
    
    /*IMU_update(0.01f, Gyro, Accel, &imu);
    Filter_Angle_s32[x] = (s32)(imu.rol * 100);
    Filter_Angle_s32[y] = (s32)(imu.pit * 100);
    Filter_Angle_s32[z] = (s32)(imu.yaw * 100);*/
}
int main(void) 
{
    u16 ir, ps, als;
    RTC_Struct rtc_structure;
    char buf[160]; 
    static u8 state = OFF;
    memset(buf, 0, sizeof(buf));
    
    //开启硬件浮点运算及ENON
    imx6ul_hardfpu_enable(); 
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
    epit_init(EPIT1, 0, 66000000 / 100);//1秒分成100份，就是10ms溢出	
    epit_init(EPIT2, 0, 66000000 / 1000);//1秒分成1000份，就是1ms溢出	
    //定时器初始化
    gpt_init(GPT1, 500);//高精度延时
    gpt_init(GPT2, 500);//LED高精度闪烁
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
    //ft5426_init();//一用触摸屏，中断之后,PWM的中断就不会再执行了！
    //PWM1初始化，控制LCD的背光亮度
    pwm_init(PWM1, 1000);//1000Hz的控制频率
    //adc初始化
    adc_init(ADC1);
    
    tft_lcd_struct.forecolor = LCD_BLACK;
    tft_lcd_struct.backcolor = LCD_ORANGE;

    //iic ap3216cs//////////////////////////////////////////////////////////////////////////////////////////////
    lcd_show_string(20, 10, 260, 32, 32,(char*)"PWM DEMO~");
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
    //adc//////////////////////////////////////////////////////////////////////////////////////////////
    lcd_show_string(700, 200, 216, 24, 24, (char*)"voltage :"); 
    //pwm//////////////////////////////////////////////////////////////////////////////////////////////
    lcd_show_string(700, 160, 120, 24, 24, (char*)"pwm :"); 
    //iic touch//////////////////////////////////////////////////////////////////////////////////////////////
    /*lcd_show_string(500, 110, 400, 16, 16,	(char*)"TP Num	:");  
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
    */

    lcd_show_string(350, 290, 200, 16, 16, (char*)"Roll:");  
	lcd_show_string(350, 310, 200, 16, 16, (char*)"Pitch:");  
	lcd_show_string(350, 330, 200, 16, 16, (char*)"Yaw:"); 
    while (1)
    {
        if (epit_1ms >= 5)//1ms到
        {
            epit_1ms = 0;
            icm20608_getdata();//1ms获取一次
            my_func();          
        }
        
        if (epit_500ms >= 500)//500ms到
        {
            epit_500ms = 0;
            state = !state;
            led_switch(LED0, state); 

            lcd_shownum(800, 160, tft_lcd_struct.backlight_pwm, 4, 24); 
            //lcd_shownum(800, 260, test_count, 4, 24);//测试正确
            //////////////////////////////////////////////////////////////////////////////////////////////      
            //考虑到循环里面其它函数的耗时可能有500ms了～
            rtc_getdatetime(&rtc_structure);//获取时间，一秒获取一次
            sprintf(buf, "%4d/%2d/%2d %2d:%2d:%2d",rtc_structure.year, rtc_structure.month, rtc_structure.day,
                                        rtc_structure.hour, rtc_structure.minute, rtc_structure.second);
            lcd_show_string(20, 70, 450, 32, 32,(char*)buf);//显示时间
        }   
        //////////////////////////////////////////////////////////////////////////////////////////////
        //150ms读取一次光强传感器的数据，因为iic传输比较慢    
        if (epit_150ms >= 150)
        {
            epit_150ms = 0;
            ap3216c_read_data(&ir, &ps, &als);
            lcd_shownum(100, 100, ir, 5, 24);//显示红外，接近距离，光强度
            lcd_shownum(100, 130, ps, 5, 24);
            lcd_shownum(100, 160, als, 5, 24);
            
        } 
        //////////////////////////////////////////////////////////////////////////////////////////////
        if (epit_100ms >= 100)
        {
            epit_100ms = 0;
            //icm20608_getdata();//100ms获取一次Yaw0608_structure.accel_x_adc);
            /*integer_display(50 + 70, 250, 16, icm20608_structure.accel_y_adc);
            integer_display(50 + 70, 270, 16, icm20608_structure.accel_z_adc);
            integer_display(50 + 70, 290, 16, icm20608_structure.gyro_x_adc);
            integer_display(50 + 70, 310, 16, icm20608_structure.gyro_y_adc);
            integer_display(50 + 70, 330, 16, icm20608_structure.gyro_z_adc);
            integer_display(50 + 70, 350, 16, icm20608_structure.temp_adc);*/
            //显示量程转换的实际值
            decimals_display(50 + 70 + 50, 230, 16, icm20608_structure.accel_x_act);
            decimals_display(50 + 70 + 50, 250, 16, icm20608_structure.accel_y_act);
            decimals_display(50 + 70 + 50, 270, 16, icm20608_structure.accel_z_act);
            decimals_display(50 + 70 + 50, 290, 16, icm20608_structure.gyro_x_act);
            decimals_display(50 + 70 + 50, 310, 16, icm20608_structure.gyro_y_act);
            decimals_display(50 + 70 + 50, 330, 16, icm20608_structure.gyro_z_act);
            decimals_display(50 + 70 + 50, 350, 16, icm20608_structure.temp_act);
            
            //decimals_display(420, 290, 16, Filter_Angle_s32[x]);
            //decimals_display(420, 310, 16, Filter_Angle_s32[y]);
            //decimals_display(420, 330, 16, Filter_Angle_s32[z]);
           
            
            decimals_display(420, 290, 16, Accel_Angle_s32[x]);
            decimals_display(420, 310, 16, Accel_Angle_s32[y]);
            //decimals_display(420, 330, 16, 0);

            decimals_display(480, 290, 16, Filter_Angle_s32[x]);
            decimals_display(480, 310, 16, Filter_Angle_s32[y]);
            
            adc_read(ADC1);
            decimals_display(820, 200, 24, voltage_i);          
        }    
        //////////////////////////////////////////////////////////////////////////////////////////////
        /*lcd_shownum(500 + 72, 110, ft5426_struct.point_num , 1, 16);//显示触摸点的个数
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

        lcd_draw_bigpoint(ft5426_struct.x[0], ft5426_struct.y[0], LCD_BLUE);//画四个点跟着手指动  
        */        
        //////////////////////////////////////////////////////////////////////////////////////////////
        //gpt_delay_ms(GPT1, 100);     
    }
    return 0;
}








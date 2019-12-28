#include "bsp_lcd.h"
#include "bsp_gpio.h"
#include "bsp_delay.h"
#include "stdio.h"


TFT_LCD_TYPE tft_lcd_struct;

/*
 * 函数名称：lcd_init
 * 函数描述：lcd控制器的初始化
 * 函数参数：void
 * 函数返回：void
 * 创建时间：2019.12.20
 * 修改时间：
 * 修改历史：
 */ 
void lcd_init(void)
{
	u16 lcdid = 0;
    lcdid = lcd_read_id();
	printf("LCD ID : %x\n", lcdid);

    lcd_gpio_init();

	lcd_reset();
	delay(10);
	lcd_noreset();

	if (lcdid == ATK4342)
	{
		tft_lcd_struct.height = 272;
		tft_lcd_struct.width = 480;
		tft_lcd_struct.vspw = 1;
		tft_lcd_struct.vbpd = 8;
		tft_lcd_struct.vfpd = 8;
		tft_lcd_struct.hspw = 1;
		tft_lcd_struct.hbpd = 40;
		tft_lcd_struct.hfpd = 5;
		lcd_clk_init(27, 8, 8);

	}
	else if(lcdid == ATK4384)
	{
		tft_lcd_struct.height = 480;	
		tft_lcd_struct.width = 800;
		tft_lcd_struct.vspw = 3;
		tft_lcd_struct.vbpd = 32;
		tft_lcd_struct.vfpd = 13;
		tft_lcd_struct.hspw = 48;
		tft_lcd_struct.hbpd = 88;
		tft_lcd_struct.hfpd = 40;
		lcd_clk_init(42, 4, 8);	/* 初始化LCD时钟 31.5MHz */
	} 
	else if(lcdid == ATK7084)
	{
		tft_lcd_struct.height = 480;	
		tft_lcd_struct.width = 800;
		tft_lcd_struct.vspw = 1;
		tft_lcd_struct.vbpd = 23;
		tft_lcd_struct.vfpd = 22;
		tft_lcd_struct.hspw = 1;
		tft_lcd_struct.hbpd = 46;
		tft_lcd_struct.hfpd = 210;
		lcd_clk_init(30, 3, 7);	/* 初始化LCD时钟 34.2MHz */	
	}
	else if(lcdid == ATK7016) 
	{
		tft_lcd_struct.height = 600;	
		tft_lcd_struct.width = 1024;
		tft_lcd_struct.vspw = 3;
		tft_lcd_struct.vbpd = 20;
		tft_lcd_struct.vfpd = 12;
		tft_lcd_struct.hspw = 20;
		tft_lcd_struct.hbpd = 140;
		tft_lcd_struct.hfpd = 160;
		lcd_clk_init(32, 3, 5); /* 设置51.2MHz像素时钟 */
	}
	else
	{
		//
	}
	
	tft_lcd_struct.framebuffer = LCD_FRAMEBUF_ADDR;
	tft_lcd_struct.forecolor = LCD_WHITE;
	tft_lcd_struct.backcolor = LCD_BLACK;
	tft_lcd_struct.pixsize = 4;
	tft_lcd_struct.backlight_pwm = 100;

	/*配置ｌｃｄ控制器接口参数*/
	LCDIF->CTRL = 0;

	LCDIF->CTRL |= (1 << 5) | (3 << 8) | (3 << 10) | (0 << 13) | (0 << 14) | (0 << 15)| (1 << 17)| (1 << 19);

	LCDIF->CTRL1 = 0;
	LCDIF->CTRL1 |= (7 << 16);

	LCDIF->TRANSFER_COUNT = 0;
	LCDIF->TRANSFER_COUNT = (tft_lcd_struct.height << 16) | (tft_lcd_struct.width);

	LCDIF->VDCTRL0 = 0;
	LCDIF->VDCTRL0 = (tft_lcd_struct.vspw << 0) | (1 << 20) | (1 << 21) | (1 << 24) | (0 << 25)| (0 << 26)| 
													(0 << 27)| (1 << 28)| (0 << 29);
	LCDIF->VDCTRL1 = tft_lcd_struct.vspw + tft_lcd_struct.height + tft_lcd_struct.vbpd + tft_lcd_struct.vfpd;

	LCDIF->VDCTRL2 = (tft_lcd_struct.hspw + tft_lcd_struct.width + tft_lcd_struct.hbpd + tft_lcd_struct.hfpd) |
						(tft_lcd_struct.hspw << 18);

	LCDIF->VDCTRL3 = (tft_lcd_struct.vspw + tft_lcd_struct.vbpd) |
						((tft_lcd_struct.hspw + tft_lcd_struct.hbpd) << 16);
	LCDIF->VDCTRL4 = (tft_lcd_struct.width) | (1 << 18);

	LCDIF->CUR_BUF = (u32)tft_lcd_struct.framebuffer;
	LCDIF->NEXT_BUF = (u32)tft_lcd_struct.framebuffer;

	lcd_enable();

	//可以开始操作了！
	lcd_clear(LCD_ORANGE);


}

/*
 * 函数名称：lcd_clk_init
 * 函数描述：lcd控制器的时钟树初始化,lcd_clk = 24 * loopdiv / prediv / div;
 * 函数参数：loopdiv，	　pll5_clk形成    27 ~ 54
 * 		　　prediv，	elcdif的一级分频 1 ~ 8
 * 		　　div，		elcdif的二级分频 1 ~ 8
 * 函数返回：void
 * 创建时间：2019.12.20
 * 修改时间：
 * 修改历史：
 */ 
void lcd_clk_init(u8 loopdiv, u8 prediv, u8 div)
{
	//不用小数分频
	CCM_ANALOG->PLL_VIDEO_DENOM = 0;
	CCM_ANALOG->PLL_VIDEO_NUM = 0;
							//pll5使能   1分频　　　　　倍频系数
	CCM_ANALOG->PLL_VIDEO = (1 << 13) | (2 << 19) | (loopdiv << 0); 

	CCM_ANALOG->MISC2 &= ~(3 << 30);//1分频

	//////////////////////////////////////////
	//第一级时钟源
	CCM->CSCDR2 &= ~(7 << 15);
	CCM->CSCDR2 |= (2 << 15);//选择pll5

	CCM->CSCDR2 &= ~(7 << 12);//一级分频
	CCM->CSCDR2 |= ((prediv - 1) << 12);

	CCM->CBCMR &= ~(7 << 23);//二级分频
	CCM->CBCMR |= ((div - 1) << 23);

	CCM->CSCDR2 &= ~(7 << 9);//二级时钟选择，000

}
//lcd控制器复位
void lcd_reset(void)
{
	LCDIF->CTRL |= 1 << 31;//写１复位
}

//lcd控制器结束复位
void lcd_noreset(void)
{
	LCDIF->CTRL &= ~(1 << 31);//写0禁止复位
}
//lcd控制器使能
void lcd_enable(void)
{
	LCDIF->CTRL |= 1 << 0;//bit 0写１使能lcd控制器
}

/*
 * 函数名称：lcd_read_id
 * 函数描述：读取正点原子屏幕的ID
 * 函数参数：void
 * 函数返回：ID
 * 创建时间：2019.12.20
 * 修改时间：
 * 修改历史：
 */ 
u16 lcd_read_id(void)
{
    u8 id = 0;

    //打开模拟开关，设置LCD_VSYNC为高,导通屏幕的三根引脚和lcd控制器得到三根数据线
    gpio_pin_config_t lcd_io_cfg;

    IOMUXC_SetPinMux(IOMUXC_LCD_VSYNC_GPIO3_IO03, 0);
    IOMUXC_SetPinConfig(IOMUXC_LCD_VSYNC_GPIO3_IO03, 0x10b0);

    lcd_io_cfg.direction = kGPIO_DigitalOutput;
    lcd_io_cfg.outputLogic = 1;
    gpio_init(GPIO3, 3, &lcd_io_cfg);

    //设置三个引脚为输入模式，来读取屏幕id
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA07_GPIO3_IO12, 0);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA07_GPIO3_IO12, 0xf080);

    IOMUXC_SetPinMux(IOMUXC_LCD_DATA15_GPIO3_IO20, 0);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA15_GPIO3_IO20, 0xf080);

    IOMUXC_SetPinMux(IOMUXC_LCD_DATA23_GPIO3_IO28, 0);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA23_GPIO3_IO28, 0xf080);

    lcd_io_cfg.direction = kGPIO_DigitalInput;
    gpio_init(GPIO3, 12, &lcd_io_cfg);
    gpio_init(GPIO3, 20, &lcd_io_cfg);
    gpio_init(GPIO3, 28, &lcd_io_cfg);

    id = (u8)gpio_read(GPIO3, 28);
    id |= (u8)gpio_read(GPIO3, 20) << 1;
    id |= (u8)gpio_read(GPIO3, 12) << 2;

    //开始判断
    if (id == 0)		return ATK4342;
    
    else if (id == 1)   return ATK7084;
    
    else if (id == 2)	return ATK7016;
    
    else if (id == 4) 	return ATK4384;
    
    else if (id == 5) 	return ATK1018;
    
    else  				return 0;
    
}
/*
 * IO引脚: 	LCD_DATA00 -> LCD_B0
 *			LCD_DATA01 -> LCD_B1
 *			LCD_DATA02 -> LCD_B2
 *			LCD_DATA03 -> LCD_B3
 *			LCD_DATA04 -> LCD_B4
 *			LCD_DATA05 -> LCD_B5
 *			LCD_DATA06 -> LCD_B6
 *			LCD_DATA07 -> LCD_B7
 *
 *			LCD_DATA08 -> LCD_G0
 *			LCD_DATA09 -> LCD_G1
 *			LCD_DATA010 -> LCD_G2
 *			LCD_DATA011 -> LCD_G3
 *			LCD_DATA012 -> LCD_G4
 *			LCD_DATA012 -> LCD_G4
 *			LCD_DATA013 -> LCD_G5
 *			LCD_DATA014 -> LCD_G6
 *			LCD_DATA015 -> LCD_G7
 *
 *			LCD_DATA016 -> LCD_R0
 *			LCD_DATA017 -> LCD_R1
 *			LCD_DATA018 -> LCD_R2 
 *			LCD_DATA019 -> LCD_R3
 *			LCD_DATA020 -> LCD_R4
 *			LCD_DATA021 -> LCD_R5
 *			LCD_DATA022 -> LCD_R6
 *			LCD_DATA023 -> LCD_R7
 *
 *			LCD_CLK -> LCD_CLK
 *			LCD_VSYNC -> LCD_VSYNC
 *			LCD_HSYNC -> LCD_HSYNC
 *			LCD_DE -> LCD_DE
 *			LCD_BL -> GPIO1_IO08 
 */
 /*
 * 函数名称：lcd_gpio_init
 * 函数描述：lcd数据线的初始化
 * 函数参数：void
 * 函数返回：void
 * 创建时间：2019.12.20
 * 修改时间：
 * 修改历史：
 */ 
void lcd_gpio_init(void)
{
    //背光结构体设置，高电平点亮LCD
    gpio_pin_config_t lcd_io_cfg;

    /* 1、IO初始化复用功能 */
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA00_LCDIF_DATA00,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA01_LCDIF_DATA01,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA02_LCDIF_DATA02,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA03_LCDIF_DATA03,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA04_LCDIF_DATA04,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA05_LCDIF_DATA05,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA06_LCDIF_DATA06,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA07_LCDIF_DATA07,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA08_LCDIF_DATA08,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA09_LCDIF_DATA09,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA10_LCDIF_DATA10,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA11_LCDIF_DATA11,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA12_LCDIF_DATA12,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA13_LCDIF_DATA13,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA14_LCDIF_DATA14,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA15_LCDIF_DATA15,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA16_LCDIF_DATA16,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA17_LCDIF_DATA17,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA18_LCDIF_DATA18,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA19_LCDIF_DATA19,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA20_LCDIF_DATA20,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA21_LCDIF_DATA21,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA22_LCDIF_DATA22,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA23_LCDIF_DATA23,0);

	IOMUXC_SetPinMux(IOMUXC_LCD_CLK_LCDIF_CLK,0);	
	IOMUXC_SetPinMux(IOMUXC_LCD_ENABLE_LCDIF_ENABLE,0);	
	IOMUXC_SetPinMux(IOMUXC_LCD_HSYNC_LCDIF_HSYNC,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_VSYNC_LCDIF_VSYNC,0);

	IOMUXC_SetPinMux(IOMUXC_GPIO1_IO08_GPIO1_IO08,0);/* 背光BL引脚      */

	/* 2、配置LCD IO属性	
	 *bit 16:0 HYS关闭
	 *bit [15:14]: 0 默认22K上拉
	 *bit [13]: 0 pull功能
	 *bit [12]: 0 pull/keeper使能 
	 *bit [11]: 0 关闭开路输出
	 *bit [7:6]: 10 速度100Mhz
	 *bit [5:3]: 111 驱动能力为R0/7
	 *bit [0]: 1 高转换率
	 */
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA00_LCDIF_DATA00,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA01_LCDIF_DATA01,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA02_LCDIF_DATA02,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA03_LCDIF_DATA03,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA04_LCDIF_DATA04,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA05_LCDIF_DATA05,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA06_LCDIF_DATA06,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA07_LCDIF_DATA07,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA08_LCDIF_DATA08,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA09_LCDIF_DATA09,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA10_LCDIF_DATA10,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA11_LCDIF_DATA11,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA12_LCDIF_DATA12,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA13_LCDIF_DATA13,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA14_LCDIF_DATA14,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA15_LCDIF_DATA15,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA16_LCDIF_DATA16,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA17_LCDIF_DATA17,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA18_LCDIF_DATA18,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA19_LCDIF_DATA19,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA20_LCDIF_DATA20,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA21_LCDIF_DATA21,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA22_LCDIF_DATA22,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA23_LCDIF_DATA23,0xB9);

	IOMUXC_SetPinConfig(IOMUXC_LCD_CLK_LCDIF_CLK,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_ENABLE_LCDIF_ENABLE,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_HSYNC_LCDIF_HSYNC,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_VSYNC_LCDIF_VSYNC,0xB9);

	IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO08_GPIO1_IO08,0x10b0);	/* 背光BL引脚 		*/

	/* GPIO初始化 */
	lcd_io_cfg.direction = kGPIO_DigitalOutput;			/* 输出 			*/
	lcd_io_cfg.outputLogic = 1; 							/* 默认关闭背光 */
	gpio_init(GPIO1, 8, &lcd_io_cfg);						/* 背光默认打开 */
	gpio_write(GPIO1, 8, 1);								/* 打开背光     */
}


inline void lcd_drawpoint(u16 x, u16 y, u32 color)
{
	*(u32*)((u32)(tft_lcd_struct.framebuffer) + tft_lcd_struct.pixsize * (tft_lcd_struct.width * y + x)) = color;
}

inline u32 lcd_readpoint(u16 x, u16 y)
{
	return *(u32*)((u32)(tft_lcd_struct.framebuffer) + tft_lcd_struct.pixsize * (tft_lcd_struct.width * y + x));
}

void lcd_clear(u32 color)
{
	u32 total_pixel = 0, i = 0;
	u32 * start_addr = (u32 *)tft_lcd_struct.framebuffer;

	total_pixel = tft_lcd_struct.width * tft_lcd_struct.height;

	for ( i = 0; i < total_pixel; i++)
	{
		start_addr[i] = color;
	}
}


/*
 * @description		: 以指定的颜色填充一块矩形
 * @param - x0		: 矩形起始点坐标X轴
 * @param - y0		: 矩形起始点坐标Y轴
 * @param - x1		: 矩形终止点坐标X轴
 * @param - y1		: 矩形终止点坐标Y轴
 * @param - color	: 要填充的颜色
 * @return 			: 读取到的指定点的颜色值
 */
void lcd_fill(u16 x0, u16 y0, u16 x1, u16 y1, u32 color)
{ 
    unsigned short x, y;

	if(x0 < 0) x0 = 0;
	if(y0 < 0) y0 = 0;
	if(x1 >= tft_lcd_struct.width) x1 = tft_lcd_struct.width - 1;
	if(y1 >= tft_lcd_struct.height) y1 = tft_lcd_struct.height - 1;
	
    for(y = y0; y <= y1; y++)
    {
        for(x = x0; x <= x1; x++)
			lcd_drawpoint(x, y, color);
    }
}
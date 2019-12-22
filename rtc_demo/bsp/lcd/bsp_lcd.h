#ifndef __BSP_LCD_H
#define __BSP_LCD_H

#include "imx6ul.h"

/* 屏幕ID */
#define ATK4342 0x4342
#define ATK7084 0x7084
#define ATK4384 0x4384
#define ATK1018 0x1018
#define ATK7016 0x7016

/* 颜色 */
#define LCD_BLUE		  0x000000FF
#define LCD_GREEN		  0x0000FF00
#define LCD_RED 		  0x00FF0000
#define LCD_CYAN		  0x0000FFFF
#define LCD_MAGENTA 	  0x00FF00FF
#define LCD_YELLOW		  0x00FFFF00
#define LCD_LIGHTBLUE	  0x008080FF
#define LCD_LIGHTGREEN	  0x0080FF80
#define LCD_LIGHTRED	  0x00FF8080
#define LCD_LIGHTCYAN	  0x0080FFFF
#define LCD_LIGHTMAGENTA  0x00FF80FF
#define LCD_LIGHTYELLOW   0x00FFFF80
#define LCD_DARKBLUE	  0x00000080
#define LCD_DARKGREEN	  0x00008000
#define LCD_DARKRED 	  0x00800000
#define LCD_DARKCYAN	  0x00008080
#define LCD_DARKMAGENTA   0x00800080
#define LCD_DARKYELLOW	  0x00808000
#define LCD_WHITE		  0x00FFFFFF
#define LCD_LIGHTGRAY	  0x00D3D3D3
#define LCD_GRAY		  0x00808080
#define LCD_DARKGRAY	  0x00404040
#define LCD_BLACK		  0x00000000
#define LCD_BROWN		  0x00A52A2A
#define LCD_ORANGE		  0x00FFA500

#define LCD_TRANSPARENT   0x00000000

typedef struct TFT_LCD_TYPE
{
    u16 height;//屏幕高
    u16 width;//宽
    u8  pixsize;//像素占几个字节

    u16 vspw;//场中断信号宽度的时钟周期
    u16 vbpd;//场中断信号的后肩时钟周期
    u16 vfpd;//场中断信号的前肩

    u16 hspw;//行中断信号宽度的时钟周期
    u16 hbpd;//行中断信号的后肩时钟周期
    u16 hfpd;//行中断信号的前肩时钟周期

    u32 framebuffer;//显存起始地址
    u32 forecolor;//前景色
    u32 backcolor;//背景色

}TFT_LCD_TYPE;
extern TFT_LCD_TYPE tft_lcd_struct;

#define LCD_FRAMEBUF_ADDR (0x89000000)

//LCD初始化工作
void lcd_init(void);
//读取屏幕IDs
u16 lcd_read_id(void);

void lcd_gpio_init(void);

void lcd_reset(void);

void lcd_noreset(void);

void lcd_enable(void);

void lcd_clk_init(u8 loopdiv, u8 prediv, u8 div);

inline void lcd_drawpoint(u16 x, u16 y, u32 color);
inline u32 lcd_readpoint(u16 x, u16 y);
void lcd_clear(u32 color);


void lcd_fill(u16 x0, u16 y0, u16 x1, u16 y1, u32 color);
#endif // !__BSP_LCD_H

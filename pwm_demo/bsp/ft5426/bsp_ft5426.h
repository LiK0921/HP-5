#ifndef __BSP_FT5426_H
#define __BSP_FT5426_H
#include "imx6ul.h"

/*寄存器地址宏定义*/
#define FT5426_ADDR				0X38	/* FT5426设备地址 		*/

#define FT5426_DEVICE_MODE		0X00 	/* 模式寄存器 			*/
#define FT5426_IDGLIB_VERSION	0XA1 	/* 固件版本寄存器 			*/
#define FT5426_IDG_MODE			0XA4	/* 中断模式				*/
#define FT5426_TD_STATUS		0X02	/* 触摸状态寄存器 			*/
#define FT5426_TOUCH1_XH		0X03	/* 触摸点坐标寄存器,一个触摸点用4个寄存器存储坐标数据*/
										 
#define FT5426_XYCOORDREG_NUM	30		/* 触摸点坐标寄存器数量 */
#define FT5426_INIT_FINISHED	1		/* 触摸屏初始化完成 			*/
#define FT5426_INIT_NOTFINISHED	0		/* 触摸屏初始化未完成 			*/

#define FT5426_TOUCH_EVENT_DOWN			0x00	/* 按下 		*/
#define FT5426_TOUCH_EVENT_UP			0x01	/* 释放 		*/
#define FT5426_TOUCH_EVENT_ON			0x02	/* 接触 		*/
#define FT5426_TOUCH_EVENT_RESERVED		0x03	/* 没有事件 */


/* 触摸信息结构体 */
typedef struct ft5426_structure 
{
	u8 initflag;	/* 触摸屏初始化状态 */
	u8 point_num;	/* 触摸点数量 */
	u16 x[5];		/* 5点X轴坐标 */
	u16 y[5];		/* 5点Y轴坐标 */
}FT5426_Structure;

extern FT5426_Structure ft5426_struct;




void ft5426_init(void);
u8 ft5426_read_onebyte(u8 addr, u8 reg);

void ft5426_read_len(u8 addr, u8 reg, u8 * buf, u8 len);

u8 ft5426_write_onebyte(u8 addr, u8 reg, u8 data);
void ft5426_read_xy(void);

void gpio1_09_irqhandle(u32 giccIar, void * user_param);

#endif // !__BSP_FT5426_H
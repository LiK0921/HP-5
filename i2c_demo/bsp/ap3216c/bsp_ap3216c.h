#ifndef __BSP_AP3216C_H
#define __BSP_AP3216C_H

#include "imx6ul.h"

//寄存器地址值
#define AP3216C_ADDR        0x1E

#define AP3216C_SYS_CONG        0x00
#define AP3216C_INT_STATUS      0x01
#define AP3216C_INT_CLEAR       0x02

#define AP3216C_IR_DATA_LOW     0x0A
#define AP3216C_IR_DATA_HIGH    0x0B

#define AP3216C_ALS_DATA_LOW     0x0C
#define AP3216C_ALS_DATA_HIGH    0x0D

#define AP3216C_PS_DATA_LOW      0x0E
#define AP3216C_PS_DATA_HIGH     0x0F

//寄存器配置值
#define AP3216C_RESET_BIT           0x4


u8 ap3216c_init(void);
u8 ap3216c_read_onebyte(u8 addr, u8 reg);
u8 ap3216c_write_onebyte(u8 addr, u8 reg, u8 data);

void ap3216c_read_data(u16 * ir, u16 * ps, u16 * als);

#endif // !__BSP_AP3216C_H
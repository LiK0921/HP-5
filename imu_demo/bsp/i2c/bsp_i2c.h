#ifndef __BSP_I2C_H
#define __BSP_I2C_H
#include "imx6ul.h"


//定义相关宏
#define I2C_STATUS_OK       (0) 
#define I2C_STATUS_BUSY     (1)
#define I2C_STATUS_IDEL     (2)
#define I2C_STATUS_NAK      (3)
#define I2C_STATUS_ARBITRATIONLOST  (4) //仲裁丢失
#define I2C_STATUS_TIMEOUT          (5)
#define I2C_STATUS_ADDRNAK          (6)

/* 传输方向结构体 */
typedef enum i2c_direction
{
    kI2C_Write = 0x0,//主机向从机写数据
    kI2C_Read  = 0x1//读数据
}I2C_Direction;


/*
 * 主机传输结构体
 */
typedef struct i2c_transfer
{
    u8 slaveAddress;      	/* 7位从机地址 			*/
    I2C_Direction direction; 		/* 传输方向 			*/
    u32 subaddress;       		/* 寄存器地址			*/
    u8 subaddressSize;    	/* 寄存器地址长度 			*/
    u8 * volatile data;    	/* 数据缓冲区 			*/
    volatile u32 dataSize;  	/* 数据缓冲区长度 			*/
}I2C_Transfer;

void i2c_init(I2C_Type * base);

u8 i2c_master_start(I2C_Type *base, u8 addr, I2C_Direction dir);

u8 i2c_master_stop(I2C_Type *base);

u8 i2c_master_start(I2C_Type *base, u8 addr, I2C_Direction dir);

u8 i2c_master_repeated_start(I2C_Type *base, u8 addr, I2C_Direction dir);

u8 i2c_check_and_clear_err(I2C_Type *base, u32 status);


void i2c_master_write(I2C_Type *base, u8 * buf, u32 size);

void i2c_master_read(I2C_Type *base, u8 * buf, u32 size);

u8 i2c_master_transfer(I2C_Type *base, I2C_Transfer *xfer);

#endif // !__BSP_I2C_H


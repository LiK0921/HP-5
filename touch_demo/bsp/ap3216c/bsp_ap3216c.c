#include "bsp_ap3216c.h"
#include "bsp_i2c.h"
#include "bsp_gpt.h"
#include "stdio.h"

u8 ap3216c_init(void)
{
    u8 test_data = 0;
    //IO初始化,复用功能
    IOMUXC_SetPinMux(IOMUXC_UART4_RX_DATA_I2C1_SDA, 1);
    IOMUXC_SetPinMux(IOMUXC_UART4_TX_DATA_I2C1_SCL, 1);
    //电气属性
    IOMUXC_SetPinConfig(IOMUXC_UART4_TX_DATA_I2C1_SCL, 0x70b0);
    IOMUXC_SetPinConfig(IOMUXC_UART4_RX_DATA_I2C1_SDA, 0x70b0);
    
    //初始化i2c控制器
    i2c_init(I2C1);
    
    //传感器初始化
    ap3216c_write_onebyte(AP3216C_ADDR, AP3216C_SYS_CONG, AP3216C_RESET_BIT);

    gpt_delay_ms(50);
    //写入一个数据0x03
    ap3216c_write_onebyte(AP3216C_ADDR, AP3216C_SYS_CONG, 0x03);
    //读取一个数据
    test_data = ap3216c_read_onebyte(AP3216C_ADDR, AP3216C_SYS_CONG);

    //printf("AP3216C_SYS_CONG is : %x\r\n", test_data);
    if (test_data != 0x03)//初始化失败
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


//ap3216c读取数据
u8 ap3216c_read_onebyte(u8 addr, u8 reg)
{
    u8 data = 0;
    I2C_Transfer i2c_transfer;

    i2c_transfer.slaveAddress = addr;
    i2c_transfer.direction    = kI2C_Read;

    i2c_transfer.subaddress = reg;
    i2c_transfer.subaddressSize = 1;

    i2c_transfer.data         = &data;
    i2c_transfer.dataSize     = 1;

    i2c_master_transfer(I2C1, &i2c_transfer);
    return data;
}

u8 ap3216c_write_onebyte(u8 addr, u8 reg, u8 data)
{
    u8 status = 0;
    I2C_Transfer i2c_transfer;

    i2c_transfer.slaveAddress = addr;
    i2c_transfer.direction    = kI2C_Write;

    i2c_transfer.subaddress = reg;
    i2c_transfer.subaddressSize = 1;

    i2c_transfer.data         = &data;
    i2c_transfer.dataSize     = 1;

    if(i2c_master_transfer(I2C1, &i2c_transfer))
        status = 1;

    return status;
}

void ap3216c_read_data(u16 * ir, u16 * ps, u16 * als)
{
    u16 buf[6];
    u8 i = 0;

    for ( i = 0; i < 6; i++)
    {
        buf[i] = ap3216c_read_onebyte(AP3216C_ADDR, AP3216C_IR_DATA_LOW + i);
    }

    if (buf[0] == (1 << 7))//表示ps和ir数据无效
    {
        * ir = 0;
        * ps = 0;
    }
    else
    {
        * ir = (buf[1] << 2) | (buf[0] & 0x03);//buf[0]bit[1:0]为低两位,buf[1]为高八位
        * ps = ((buf[5] & 0x3f) << 4) | (buf[4] & 0x0f);
    
    }
    * als = (buf[3] << 8) | (buf[2]);
}



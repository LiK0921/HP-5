#include "bsp_ft5426.h"
#include "bsp_gpio.h"
#include "bsp_int.h"
#include "bsp_gpt.h"
#include "bsp_i2c.h"
#include "stdio.h"

FT5426_Structure ft5426_struct;

/*初始化*/
void ft5426_init(void)
{
    u8 regbuf[2];
    //io
    gpio_pin_config_t int_config, cs_config;
    int_config.direction = kGPIO_DigitalInput;
    int_config.interrupt_mode = kGPIO_IntRisingOrFallingEdge;//双边缘触发
    int_config.outputLogic = 1;

    cs_config.direction = kGPIO_DigitalOutput;
    cs_config.interrupt_mode = kGPIO_NoIntmode;
    cs_config.outputLogic = 1;
   
    //触摸屏中断引脚
    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO09_GPIO1_IO09, 0);
    //设置电气属性
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO09_GPIO1_IO09, 0xf080);

    gpio_init(GPIO1, 9, &int_config);

    //使能指定的中断
    GIC_EnableIRQ(GPIO1_Combined_0_15_IRQn);
    //将按键对应的gpio脚注册进去
    sys_register_irqhandle(GPIO1_Combined_0_15_IRQn, (system_irq_handler_t)gpio1_09_irqhandle, NULL);
    //开启中断
    gpio_int_enble(GPIO1, 9);

    //触摸屏使能引脚
    IOMUXC_SetPinMux(IOMUXC_SNVS_SNVS_TAMPER9_GPIO5_IO09, 0);
    //设置电气属性
    IOMUXC_SetPinConfig(IOMUXC_SNVS_SNVS_TAMPER9_GPIO5_IO09, 0x10b0);

    gpio_init(GPIO5, 9, &cs_config);

    gpio_write(GPIO5, 9, 0);//复位
    gpt_delay_ms(50);
    gpio_write(GPIO5, 9, 1);//停止复位
    gpt_delay_ms(50);

    //IO初始化,复用功能
    IOMUXC_SetPinMux(IOMUXC_UART5_RX_DATA_I2C2_SDA, 1);
    IOMUXC_SetPinMux(IOMUXC_UART5_TX_DATA_I2C2_SCL, 1);
    //电气属性
    IOMUXC_SetPinConfig(IOMUXC_UART5_RX_DATA_I2C2_SDA, 0x70b0);
    IOMUXC_SetPinConfig(IOMUXC_UART5_TX_DATA_I2C2_SCL, 0x70b0);
    
    //初始化i2c控制器
    i2c_init(I2C2);

    //ft5426寄存器配置初始化
    //读取软件版本号,两个字节
    ft5426_read_len(FT5426_ADDR, FT5426_IDGLIB_VERSION, regbuf, 2);
    printf("ft5426 fireware version is %#X\r\n", (regbuf[0] << 8) | (regbuf[1]));

    ft5426_write_onebyte(FT5426_ADDR, FT5426_DEVICE_MODE, 0);//正常运行
    ft5426_write_onebyte(FT5426_ADDR, FT5426_IDG_MODE, 1);//中断模式

    ft5426_struct.initflag = FT5426_INIT_FINISHED;

    //Interrupt

}

//触摸屏中断处理函数
void gpio1_09_irqhandle(u32 giccIar, void * user_param)
{
    if (ft5426_struct.initflag)//初始化完成
    {
        ft5426_read_xy();
    }
    
    //清除中断标记位
    gpio_int_clearflag(GPIO1, 9);
}

u8 ft5426_write_onebyte(u8 addr, u8 reg, u8 data)
{
    u8 status = 0;
    I2C_Transfer i2c_transfer;

    i2c_transfer.slaveAddress = addr;
    i2c_transfer.direction    = kI2C_Write;

    i2c_transfer.subaddress = reg;
    i2c_transfer.subaddressSize = 1;

    i2c_transfer.data         = &data;
    i2c_transfer.dataSize     = 1;

    if(i2c_master_transfer(I2C2, &i2c_transfer))
        status = 1;

    return status;
}

u8 ft5426_read_onebyte(u8 addr, u8 reg)
{
    u8 data = 0;
    I2C_Transfer i2c_transfer;

    i2c_transfer.slaveAddress = addr;
    i2c_transfer.direction    = kI2C_Read;

    i2c_transfer.subaddress = reg;
    i2c_transfer.subaddressSize = 1;

    i2c_transfer.data         = &data;
    i2c_transfer.dataSize     = 1;

    i2c_master_transfer(I2C2, &i2c_transfer);
    return data;
}


void ft5426_read_len(u8 addr, u8 reg, u8 * buf, u8 len)
{
    I2C_Transfer i2c_transfer;

    i2c_transfer.slaveAddress = addr;
    i2c_transfer.direction    = kI2C_Read;

    i2c_transfer.subaddress = reg;
    i2c_transfer.subaddressSize = 1;

    i2c_transfer.data         = buf;
    i2c_transfer.dataSize     = len;

    i2c_master_transfer(I2C2, &i2c_transfer);
}

void ft5426_read_xy(void)
{
    u8 i = 0, type = 0;
    u8 databuf[FT5426_XYCOORDREG_NUM];
    u8 * pBuf = 0;
    //获取触摸数量
    ft5426_struct.point_num = ft5426_read_onebyte(FT5426_ADDR, FT5426_TD_STATUS);//bit[3:0]记录接触点数目

    ft5426_read_len(FT5426_ADDR, FT5426_TOUCH1_XH, databuf, FT5426_XYCOORDREG_NUM);

    for ( i = 0; i < ft5426_struct.point_num; i++)
    {
        //因为一个点有六个寄存器，其中后两个是保留的，前四个是坐标值，x和y各有12bit表示
        pBuf = &databuf[i * 6];
        ft5426_struct.y[i] = ((pBuf[0] << 8) | pBuf[1]) & 0xFFF;//FT5426_TOUCH1_XH的bit[3:0]保存着x坐标的bit[11:8]
        ft5426_struct.x[i] = ((pBuf[2] << 8) | pBuf[3]) & 0xFFF;//FT5426_TOUCH1_XL的bit[7:0]保存着x坐标的bit[7:0]
    
        type = databuf[0] >> 6;//bit[7:6]是事件标记位
        if (type == FT5426_TOUCH_EVENT_DOWN)//按下
        {
            /* code */
        }
        else if (type == FT5426_TOUCH_EVENT_UP)//释放
        {
            
        }
        else if (type == FT5426_TOUCH_EVENT_ON)//接触
        {

        }
        else//无事件
        {
            /* code */
        }
    }
}




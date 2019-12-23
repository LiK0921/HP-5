#include "bsp_icm20608.h"
#include "bsp_spi.h"
#include "bsp_gpt.h"
#include "stdio.h"

ICM20608_Structure icm20608_structure;
//量程
float   gyroscale;
u16     accescale;

u8 icm20608_init(void)
{
    u8 who_am_i = 0;
    gpio_pin_config_t cs_config;

    //引脚初始化
    //IO初始化,复用功能
    IOMUXC_SetPinMux(IOMUXC_UART2_RX_DATA_ECSPI3_SCLK, 0);
    IOMUXC_SetPinMux(IOMUXC_UART2_CTS_B_ECSPI3_MOSI, 0);
    IOMUXC_SetPinMux(IOMUXC_UART2_RTS_B_ECSPI3_MISO, 0);
    //电气属性
    IOMUXC_SetPinConfig(IOMUXC_UART2_CTS_B_ECSPI3_MOSI, 0x10b0);
    IOMUXC_SetPinConfig(IOMUXC_UART2_RX_DATA_ECSPI3_SCLK, 0x10b0);
    IOMUXC_SetPinConfig(IOMUXC_UART2_RTS_B_ECSPI3_MISO, 0x10b0);
    
    //片选初始化
    IOMUXC_SetPinMux(IOMUXC_UART2_TX_DATA_GPIO1_IO20, 0);
    IOMUXC_SetPinConfig(IOMUXC_UART2_TX_DATA_GPIO1_IO20, 0x10b0);
   
    
    cs_config.direction = kGPIO_DigitalOutput;
    cs_config.outputLogic = 0;
    gpio_init(GPIO1, 20, &cs_config);

    //

    //spi3控制器初始化
    spi_init(ECSPI3);

    //icm初始化
    //复位
    icm20608_write_reg(ECSPI3, ICM20_PWR_MGMT_1, 0x80);
    gpt_delay_ms(50);
    //关闭睡眠模式
    icm20608_write_reg(ECSPI3, ICM20_PWR_MGMT_1, 0x01);
    gpt_delay_ms(50);



    //读取ID, 0xAF或0xAE
    who_am_i = icm20608_read_reg(ECSPI3, ICM20_WHO_AM_I);
    printf("who an i reg : %#X\r\n", who_am_i);//0xAE

    //配置量程
    icm20608_write_reg(ECSPI3,ICM20_SMPLRT_DIV, 0x00); 	/* 输出速率是内部采样率					*/
	icm20608_write_reg(ECSPI3,ICM20_GYRO_CONFIG, 0x18); 	/* 陀螺仪±2000dps量程 				*/
	icm20608_write_reg(ECSPI3,ICM20_ACCEL_CONFIG, 0x18); 	/* 加速度计±16G量程 					*/
	icm20608_write_reg(ECSPI3,ICM20_CONFIG, 0x04); 		/* 陀螺仪低通滤波BW=20Hz 				*/
	icm20608_write_reg(ECSPI3,ICM20_ACCEL_CONFIG2, 0x04); 	/* 加速度计低通滤波BW=21.2Hz 			*/
	icm20608_write_reg(ECSPI3,ICM20_PWR_MGMT_2, 0x00); 	/* 打开加速度计和陀螺仪所有轴 				*/
	icm20608_write_reg(ECSPI3,ICM20_LP_MODE_CFG, 0x00); 	/* 关闭低功耗 						*/
	icm20608_write_reg(ECSPI3,ICM20_FIFO_EN, 0x00);		/* 关闭FIFO						*/

    //获取量程
    gyroscale = icm20608_gyro_scaleget();
	accescale = icm20608_accel_scaleget();

    return 0;
}


/*
    icm读数据
*/
u8 icm20608_read_reg(ECSPI_Type * base, u8 reg)
{
    u8 rxdata = 0;
    reg |= 0x80;//第8位置1

    ICM20608_CSN(0);//选中
    //写入寄存器地址
    spich_0_read_write(base, reg);
    //读取寄存器的数据，写0xFF只是提供时钟给从机
    rxdata = spich_0_read_write(base, 0xFF);

    ICM20608_CSN(1);//停止访问

    return rxdata;
}


/*
    icm写数据
*/
void icm20608_write_reg(ECSPI_Type * base, u8 reg, u8 txdata)
{

    reg &= 0x7F;//第8位置0

    ICM20608_CSN(0);//选中
    //写入寄存器地址
    spich_0_read_write(base, reg);
    //写寄存器的数据
    spich_0_read_write(base, txdata);

    ICM20608_CSN(1);//停止访问
}

//读取多个寄存器的值
void icm20608_read_buf(ECSPI_Type * base, u8 reg, u8 * buf, u8 len)
{
    u8 i = 0;
    reg |= 0x80;//第8位置1
    ICM20608_CSN(0);//选中
    //写入寄存器地址
    spich_0_read_write(base, reg);
    for ( i = 0; i < len; i++)
    {
        //读取寄存器的数据，写0xFF只是提供时钟给从机
        buf[i] = spich_0_read_write(base, 0xFF);
    }
    ICM20608_CSN(1);//停止访问
}


/*
 * @description	: 使能I.MX6U的硬件NEON和FPU
 * @param 		: 无
 * @return 		: 无
 */
void imx6ul_hardfpu_enable(void)
{
	uint32_t cpacr;
	uint32_t fpexc;

	/* 使能NEON和FPU */
	cpacr = __get_CPACR();
	cpacr = (cpacr & ~(CPACR_ASEDIS_Msk | CPACR_D32DIS_Msk))
		   |  (3UL << CPACR_cp10_Pos) | (3UL << CPACR_cp11_Pos);
	__set_CPACR(cpacr);

	fpexc = __get_FPEXC();
	fpexc |= 0x40000000UL;	
	__set_FPEXC(fpexc);
}

/*
 * @description : 获取陀螺仪的分辨率
 * @param		: 无
 * @return		: 获取到的分辨率
 */
float icm20608_gyro_scaleget(void)
{
	u8 data;
	float gyroscale;
	
	data = (icm20608_read_reg(ECSPI3, ICM20_GYRO_CONFIG) >> 3) & 0X3;
	switch(data) {
		case 0: 
			gyroscale = 131;
			break;
		case 1:
			gyroscale = 65.5;
			break;
		case 2:
			gyroscale = 32.8;
			break;
		case 3:
			gyroscale = 16.4;
			break;
	}
	return gyroscale;
}

/*
 * @description : 获取加速度计的分辨率
 * @param		: 无
 * @return		: 获取到的分辨率
 */
u16 icm20608_accel_scaleget(void)
{
	u8 data;
	u16 accelscale;
	
	data = (icm20608_read_reg(ECSPI3, ICM20_ACCEL_CONFIG) >> 3) & 0X3;
	switch(data) {
		case 0: 
			accelscale = 16384;
			break;
		case 1:
			accelscale = 8192;
			break;
		case 2:
			accelscale = 4096;
			break;
		case 3:
			accelscale = 2048;
			break;
	}
	return accelscale;
}


//获取icm内部数据
void icm20608_getdata(void)
{
    u8 buf[14];

    icm20608_read_buf(ECSPI3, ICM20_ACCEL_XOUT_H, buf, 14);

    /* ADC传感器数据 */
    icm20608_structure.accel_x_adc = (signed short)((buf[0] << 8)| buf[1]);
    icm20608_structure.accel_y_adc = (signed short)((buf[2] << 8) | buf[3]); 
	icm20608_structure.accel_z_adc = (signed short)((buf[4] << 8) | buf[5]); 

	icm20608_structure.temp_adc    = (signed short)((buf[6] << 8) | buf[7]); 

	icm20608_structure.gyro_x_adc  = (signed short)((buf[8] << 8) | buf[9]); 
	icm20608_structure.gyro_y_adc  = (signed short)((buf[10] << 8) | buf[11]);
	icm20608_structure.gyro_z_adc  = (signed short)((buf[12] << 8) | buf[13]);

    /* 计算实际数据，实际数据扩大100，0.01扩大100=1 */
	icm20608_structure.gyro_x_act = ((float)(icm20608_structure.gyro_x_adc)  / gyroscale) * 100;
	icm20608_structure.gyro_y_act = ((float)(icm20608_structure.gyro_y_adc)  / gyroscale) * 100;
	icm20608_structure.gyro_z_act = ((float)(icm20608_structure.gyro_z_adc)  / gyroscale) * 100;

	icm20608_structure.accel_x_act = ((float)(icm20608_structure.accel_x_adc) / accescale) * 100;
	icm20608_structure.accel_y_act = ((float)(icm20608_structure.accel_y_adc) / accescale) * 100;
	icm20608_structure.accel_z_act = ((float)(icm20608_structure.accel_z_adc) / accescale) * 100;
    //计算实际温度值 * 100以便显示
	icm20608_structure.temp_act = (((float)(icm20608_structure.temp_adc) - 25 ) / 326.8 + 25) * 100;
}


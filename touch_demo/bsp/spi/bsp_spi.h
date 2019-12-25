#ifndef __BSP_SPI_H
#define __BSP_SPI_H
#include "imx6ul.h"

//spi控制器初始化
void spi_init(ECSPI_Type * base);

u8 spich_0_read_write(ECSPI_Type * base, u8 txdata);

#endif // !__BSP_SPI_H
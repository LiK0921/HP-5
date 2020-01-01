#include "bsp_spi.h"

//spi控制器初始化
void spi_init(ECSPI_Type * base)
{
    base->CONREG = 0;
    base->CONREG |= (1 << 0) |(1 << 3) | (1 << 4) | (7 << 20);
    
    base->CONFIGREG = 0;

    base->PERIODREG = 0x2000;

    //spi内部时钟分频
    base->CONREG &= ~((0xF << 12) | (0xF << 8));//清零
    base->CONREG |= (9 << 12) | (0 << 8);//1级分频10分频，2级分频1分频

}

//spi发送与接收函数
u8 spich_0_read_write(ECSPI_Type * base, u8 txdata)
{
    u32 rxdata = 0;

    base->CONREG &= ~(3 << 18);//清零
    base->CONREG |= (0 << 18);//选择通道0

    //等待发送完成
    while ((base->STATREG & (1 << 0)) == 0)
    {
        /* code */
    }
    base->TXDATA = txdata;
    
    //等待数据接收
    while ((base->STATREG & (1 << 3)) == 0)
    {
        /* code */
    }
    rxdata = base->RXDATA;

    return rxdata;
}

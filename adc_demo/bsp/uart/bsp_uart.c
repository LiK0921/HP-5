#include "bsp_uart.h"
#include "bsp_gpio.h"

void uart_init(UART_Type * base, u32 broud)
{
    uart_io_init(base);

    uart_disable(UART1);

    uart_soft_reset(UART1);

    base->UCR1 = 0;
    base->UCR1 &= ~(1 << 14);//关闭波特率自检

    /*
    * 设置 UART 的 UCR2 寄存器，设置字长，停止位，校验模式，关闭硬件流控
    * bit14: 1 忽略 RTS 引脚
    * bit8: 0 关闭奇偶校验
    * bit6: 0 1 位停止位
    * bit5: 1 8 位数据位
    * bit2: 1 打开发送
    * bit1: 1 打开接收
    */
    base->UCR2 = 0;
    base->UCR2 |= (1 << 1) | (1 << 2)| (1 << 5) | (1 << 14);

    //base->UCR3 = 0;
    base->UCR3 |= (1 << 2);//bit2必须为１

    /*
    * 设置波特率
    * 波特率计算公式:Baud Rate = Ref Freq / (16 * (UBMR + 1)/(UBIR+1)) 
    * 如果要设置波特率为 115200，那么可以使用如下参数:
    * Ref Freq = 80M 也就是寄存器 UFCR 的 bit9:7=101, 表示 1 分频
    * UBMR = 3124
    * UBIR = 71
    * 因此波特率= 80000000/(16 * (3124+1)/(71+1))
    * = 80000000/(16 * 3125/72) 
    * = (80000000*72) / (16*3125) 
    * = 115200
    */

#if 0
    //配置波特率为115200
    base->UFCR &= ~(7 << 7);
    base->UFCR |= (5 << 7);
   
    base->UBIR = 71;
    base->UBMR = 3124;
#endif // 0

    uart_setbaudrate(UART1, 115200, 80000000);

    uart_enable(UART1);

}
void uart_io_init(UART_Type *base)
{  
    if (base == UART1)
    {
        /* 1、初始化IO复用 
        * UART1_RXD -> UART1_TX_DATA
        * UART1_TXD -> UART1_RX_DATA
        */
        IOMUXC_SetPinMux(IOMUXC_UART1_TX_DATA_UART1_TX, 0);	/* 复用为UART1_TX */
	    IOMUXC_SetPinMux(IOMUXC_UART1_RX_DATA_UART1_RX, 0);	/* 复用为UART1_RX */

        /* 2、配置UART1_TX_DATA、UART1_RX_DATA的IO属性 
        *bit 16:0 HYS关闭
        *bit [15:14]: 00 默认100K下拉
        *bit [13]: 0 keeper功能
        *bit [12]: 1 pull/keeper使能
        *bit [11]: 0 关闭开路输出
        *bit [7:6]: 10 速度100Mhz
        *bit [5:3]: 110 驱动能力R0/6
        *bit [0]: 0 低转换率
        */
        IOMUXC_SetPinConfig(IOMUXC_UART1_TX_DATA_UART1_TX, 0x10B0);
        IOMUXC_SetPinConfig(IOMUXC_UART1_RX_DATA_UART1_RX, 0x10B0);
    }	
    else if (base == UART2)
    {
        IOMUXC_SetPinMux(IOMUXC_UART2_TX_DATA_UART2_TX, 0);	/* 复用为UART1_TX */
	    IOMUXC_SetPinMux(IOMUXC_UART2_RX_DATA_UART2_RX, 0);	/* 复用为UART1_RX */
        IOMUXC_SetPinConfig(IOMUXC_UART2_TX_DATA_UART2_TX, 0x10B0);//电气属性
        IOMUXC_SetPinConfig(IOMUXC_UART2_RX_DATA_UART2_RX, 0x10B0);//电气属性
    }
    else if (base == UART3)
    {
        IOMUXC_SetPinMux(IOMUXC_UART3_TX_DATA_UART3_TX, 0);	/* 复用为UART1_TX */
	    IOMUXC_SetPinMux(IOMUXC_UART3_RX_DATA_UART3_RX, 0);	/* 复用为UART1_RX */
        IOMUXC_SetPinConfig(IOMUXC_UART3_TX_DATA_UART3_TX, 0x10B0);//电气属性
        IOMUXC_SetPinConfig(IOMUXC_UART3_RX_DATA_UART3_RX, 0x10B0);//电气属性
    }
    else if (base == UART4)
    {
        IOMUXC_SetPinMux(IOMUXC_UART4_TX_DATA_UART4_TX, 0);	/* 复用为UART1_TX */
	    IOMUXC_SetPinMux(IOMUXC_UART4_RX_DATA_UART4_RX, 0);	/* 复用为UART1_RX */
        IOMUXC_SetPinConfig(IOMUXC_UART4_TX_DATA_UART4_TX, 0x10B0);//电气属性
        IOMUXC_SetPinConfig(IOMUXC_UART4_RX_DATA_UART4_RX, 0x10B0);//电气属性
    }
    else if (base == UART5)
    {
        IOMUXC_SetPinMux(IOMUXC_UART5_TX_DATA_UART5_TX, 0);	/* 复用为UART1_TX */
	    IOMUXC_SetPinMux(IOMUXC_UART5_RX_DATA_UART5_RX, 0);	/* 复用为UART1_RX */
        IOMUXC_SetPinConfig(IOMUXC_UART5_TX_DATA_UART5_TX, 0x10B0);//电气属性
        IOMUXC_SetPinConfig(IOMUXC_UART5_RX_DATA_UART5_RX, 0x10B0);//电气属性
    }
    else if (base == UART6)
    {
        IOMUXC_SetPinMux(IOMUXC_ENET2_RX_DATA0_UART6_TX, 0);	/* 复用为UART1_TX */
	    IOMUXC_SetPinMux(IOMUXC_ENET2_RX_DATA0_UART6_RX, 0);	/* 复用为UART1_RX */
        IOMUXC_SetPinConfig(IOMUXC_ENET2_RX_DATA0_UART6_TX, 0x10B0);//电气属性
        IOMUXC_SetPinConfig(IOMUXC_ENET2_RX_DATA0_UART6_RX, 0x10B0);//电气属性
    }
    else if (base == UART7)
    {
        IOMUXC_SetPinMux(IOMUXC_ENET2_TX_DATA0_UART7_RX, 0);	/* 复用为UART1_TX */
	    IOMUXC_SetPinMux(IOMUXC_ENET2_TX_DATA0_UART7_TX, 0);	/* 复用为UART1_RX */
        IOMUXC_SetPinConfig(IOMUXC_ENET2_TX_DATA0_UART7_RX, 0x10B0);//电气属性
        IOMUXC_SetPinConfig(IOMUXC_ENET2_TX_DATA0_UART7_TX, 0x10B0);//电气属性
    }
    else if (base == UART8)
    {
        IOMUXC_SetPinMux(IOMUXC_ENET2_TX_DATA1_UART8_TX, 0);	/* 复用为UART1_TX */
	    IOMUXC_SetPinMux(IOMUXC_ENET2_TX_DATA1_UART8_RX, 0);	/* 复用为UART1_RX */
        IOMUXC_SetPinConfig(IOMUXC_ENET2_TX_DATA1_UART8_TX, 0x10B0);//电气属性
        IOMUXC_SetPinConfig(IOMUXC_ENET2_TX_DATA1_UART8_RX, 0x10B0);//电气属性
    }
}


void uart_disable(UART_Type * base)
{
    base->UCR1 &= ~(1 << 0);
}
void uart_enable(UART_Type * base)
{
    base->UCR1 |= (1 << 0);
}

void uart_soft_reset(UART_Type * base)
{
    base->UCR2 &= ~(1 << 0);
    while (((base->UCR2 >> 0) & 0x1) == 0);
}

/*void my_putc(UART_Type * base, char c)
{
    while(((base->USR2 >> 3) &0X01) == 0);// 等待上一次发送完成 
	base->UTXD = c & 0XFF; 				// 发送数据 
}*/
void putc(u8 c)
{
    while(((Print_Port->USR2 >> 3) &0X01) == 0);/* 等待上一次发送完成 */
	Print_Port->UTXD = c & 0XFF; 				/* 发送数据 */
}

/*u8 my_getc(UART_Type * base)
{
    while(((base->USR2 >> 0) & 0x01) == 0);// 等待接收完成
	return base->URXD;				// 返回接收到的数据
}*/
u8 getc()
{
    while(((Print_Port->USR2 >> 0) & 0x01) == 0);/* 等待接收完成 */
	return Print_Port->URXD;				/* 返回接收到的数据 */
}

void puts(char * str)
{
    char * p = str;
    while (*p)
    {
        putc(*p++);
    }
}

/*void my_puts(UART_Type * base, char * str)
{
    char * p = str;
    while (*p)
    {
        my_putc(base, *p++);
    }
}*/



/*
 * @description 		: 波特率计算公式，
 *    			  	  	  可以用此函数计算出指定串口对应的UFCR，
 * 				          UBIR和UBMR这三个寄存器的值
 * @param - base		: 要计算的串口。
 * @param - baudrate	: 要使用的波特率。
 * @param - srcclock_hz	:串口时钟源频率，单位Hz
 * @return		: 无
 */
void uart_setbaudrate(UART_Type *base, unsigned int baudrate, unsigned int srcclock_hz)
{
    uint32_t numerator = 0u;		//分子
    uint32_t denominator = 0U;		//分母
    uint32_t divisor = 0U;
    uint32_t refFreqDiv = 0U;
    uint32_t divider = 1U;
    uint64_t baudDiff = 0U;
    uint64_t tempNumerator = 0U;
    uint32_t tempDenominator = 0u;

    /* get the approximately maximum divisor */
    numerator = srcclock_hz;
    denominator = baudrate << 4;
    divisor = 1;

    while (denominator != 0)
    {
        divisor = denominator;
        denominator = numerator % denominator;
        numerator = divisor;
    }

    numerator = srcclock_hz / divisor;
    denominator = (baudrate << 4) / divisor;

    /* numerator ranges from 1 ~ 7 * 64k */
    /* denominator ranges from 1 ~ 64k */
    if ((numerator > (UART_UBIR_INC_MASK * 7)) || (denominator > UART_UBIR_INC_MASK))
    {
        uint32_t m = (numerator - 1) / (UART_UBIR_INC_MASK * 7) + 1;
        uint32_t n = (denominator - 1) / UART_UBIR_INC_MASK + 1;
        uint32_t max = m > n ? m : n;
        numerator /= max;
        denominator /= max;
        if (0 == numerator)
        {
            numerator = 1;
        }
        if (0 == denominator)
        {
            denominator = 1;
        }
    }
    divider = (numerator - 1) / UART_UBIR_INC_MASK + 1;

    switch (divider)
    {
        case 1:
            refFreqDiv = 0x05;
            break;
        case 2:
            refFreqDiv = 0x04;
            break;
        case 3:
            refFreqDiv = 0x03;
            break;
        case 4:
            refFreqDiv = 0x02;
            break;
        case 5:
            refFreqDiv = 0x01;
            break;
        case 6:
            refFreqDiv = 0x00;
            break;
        case 7:
            refFreqDiv = 0x06;
            break;
        default:
            refFreqDiv = 0x05;
            break;
    }
    /* Compare the difference between baudRate_Bps and calculated baud rate.
     * Baud Rate = Ref Freq / (16 * (UBMR + 1)/(UBIR+1)).
     * baudDiff = (srcClock_Hz/divider)/( 16 * ((numerator / divider)/ denominator).
     */
    tempNumerator = srcclock_hz;
    tempDenominator = (numerator << 4);
    divisor = 1;
    /* get the approximately maximum divisor */
    while (tempDenominator != 0)
    {
        divisor = tempDenominator;
        tempDenominator = tempNumerator % tempDenominator;
        tempNumerator = divisor;
    }
    tempNumerator = srcclock_hz / divisor;
    tempDenominator = (numerator << 4) / divisor;
    baudDiff = (tempNumerator * denominator) / tempDenominator;
    baudDiff = (baudDiff >= baudrate) ? (baudDiff - baudrate) : (baudrate - baudDiff);

    if (baudDiff < (baudrate / 100) * 3)
    {
        base->UFCR &= ~UART_UFCR_RFDIV_MASK;
        base->UFCR |= UART_UFCR_RFDIV(refFreqDiv);
        base->UBIR = UART_UBIR_INC(denominator - 1); //要先写UBIR寄存器，然后在写UBMR寄存器，3592页 
        base->UBMR = UART_UBMR_MOD(numerator / divider - 1);
    }
}

//防止编译器报错
void raise(int sig_nr)
{

}


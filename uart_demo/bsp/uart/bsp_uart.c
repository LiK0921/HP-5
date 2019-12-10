#include "bsp_uart.h"
#include "bsp_gpio.h"

void uart_init(UART_Type * base, u32 broud)
{
    uart_io_init();

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

    //配置波特率为115200
    base->UFCR &= ~(7 << 7);
    base->UFCR |= (5 << 7);
   
    base->UBIR = 71;
    base->UBMR = 3124;

    uart_enable(UART1);

}
void uart_io_init(void)
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

void my_putc(UART_Type * base, char c)
{
    while(((base->USR2 >> 3) &0X01) == 0);/* 等待上一次发送完成 */
	base->UTXD = c & 0XFF; 				/* 发送数据 */
}
void my_puts(UART_Type * base, char * str)
{
    char * p = str;
    while (*p)
    {
        my_putc(base, *p++);
    }
}
u8 my_getc(UART_Type * base)
{
    while(((base->USR2 >> 0) & 0x01) == 0);/* 等待接收完成 */
	return base->URXD;				/* 返回接收到的数据 */
}

//防止编译器报错
void raise(int sig_nr)
{

}

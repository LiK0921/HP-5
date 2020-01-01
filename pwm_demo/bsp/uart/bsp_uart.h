#ifndef __BSP_UART_H
#define __BSP_UART_H
#include "imx6ul.h"

//库函数标准输出端口选择
#define Print_Port  (UART1)

void uart_init(UART_Type * base, u32 broud);
void uart_io_init(UART_Type *base);

void uart_disable(UART_Type * base);
void uart_enable(UART_Type * base);
void uart_soft_reset(UART_Type * base);


//void my_putc(UART_Type * base, char c);
void putc(u8 c);

//u8 my_getc(UART_Type * base);
u8 getc();

//void my_puts(UART_Type * base, char * str);
void puts(char * str);

void uart_setbaudrate(UART_Type *base, unsigned int baudrate, unsigned int srcclock_hz);

void raise(int sig_nr);

#endif // !__BSP_UART_H
#ifndef __BSP_UART_H
#define __BSP_UART_H
#include "imx6ul.h"


void uart_init(UART_Type * base, u32 broud);
void uart_io_init(void);

void uart_disable(UART_Type * base);
void uart_enable(UART_Type * base);
void uart_soft_reset(UART_Type * base);


void my_putc(UART_Type * base, char c);
void my_puts(UART_Type * base, char * str);
u8 my_getc(UART_Type * base);
void raise(int sig_nr);

#endif // !__BSP_UART_H
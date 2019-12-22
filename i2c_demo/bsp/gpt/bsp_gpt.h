#ifndef __BSP_GPT_H
#define __BSP_GPT_H

#include "imx6ul.h"

void gpt_delay_init(void);
void gpt_delay_us(u32 n);
void gpt_delay_ms(u32 n);

void gpt1_irqhandle(void);

#endif // !__BSP_GPT_H
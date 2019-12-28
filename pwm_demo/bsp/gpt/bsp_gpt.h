#ifndef __BSP_GPT_H
#define __BSP_GPT_H

#include "imx6ul.h"


#define GPT1_IRQ_ENABLE     1
#define GPT2_IRQ_ENABLE     1

#define GPT1_IRQ_DISABLE     0
#define GPT2_IRQ_DISABLE     0

void gpt_init(GPT_Type * base, u32 irq_ms);

void gpt_delay_us(GPT_Type * base, u32 n);
void gpt_delay_ms(GPT_Type * base, u32 n);

void gpt1_irqhandle(u32 giccIar, void * user_param);
void gpt2_irqhandle(u32 giccIar, void * user_param);

#endif // !__BSP_GPT_H
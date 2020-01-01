#ifndef __BSP_EPIT_H
#define __BSP_EPIT_H

#include "imx6ul.h"

#define EPIT1_ENABLE    1

#define EPIT2_ENABLE    1

extern u16 epit_1ms, epit_100ms, epit_150ms,epit_500ms;

void epit_init(EPIT_Type * base, u32 frac, u32 loadvalue);

void epit_stop(EPIT_Type * base);
void epit_restart(EPIT_Type * base, u32 loadvalue);


void epit1_irqhandle(u32 giccIar, void * user_param);
void epit2_irqhandle(u32 giccIar, void * user_param);
#endif // !__BSP_EPIT_H
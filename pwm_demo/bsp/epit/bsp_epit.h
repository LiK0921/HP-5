#ifndef __BSP_EPIT_H
#define __BSP_EPIT_H

#include "imx6ul.h"

#define EPIT1_ENABLE    1

void epit_init(EPIT_Type * base, u32 frac, u32 loadvalue);

void epit_stop(EPIT_Type * base);
void epit_restart(EPIT_Type * base, u32 loadvalue);


void epit1_irqhandle(u32 giccIar, void * user_param);

#endif // !__BSP_EPIT_H
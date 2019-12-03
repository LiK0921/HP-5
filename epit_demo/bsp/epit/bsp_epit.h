#ifndef __BSP_EPIT_H
#define __BSP_EPIT_H

#include "imx6ul.h"

void epit1_init(u32 frac, u32 loadvalue);

void epit1_irqhandle(void);

#endif // !__BSP_EPIT_H
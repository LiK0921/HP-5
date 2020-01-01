#ifndef __BSP_EXIT_H
#define __BSP_EXIT_H
#include "imx6ul.h"
void exit_init(void);


void gpio1_io18_irqhandle(u32 giccIar, void * user_param);

#endif // !__BSP_EXIT_H
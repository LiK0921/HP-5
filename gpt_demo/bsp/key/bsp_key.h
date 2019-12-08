#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "imx6ul.h"

enum keyvalue
{
    KEY_NONE = 0U,
    KEY0_VALUE,
	KEY1_VALUE,
	KEY2_VALUE,
};


void key_init(void);

int key_getvalue(void);

#endif // !__BSP_KEY_H
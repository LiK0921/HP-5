#ifndef __BSP_INT_H
#define __BSP_INT_H
#include "imx6ul.h"
typedef void (*system_irq_handler_t)(u32 giccIar,void * param);

/*中断服务函数结构体*/
typedef struct _sys_irp_handle
{
    system_irq_handler_t irq_handle;//中断服务函数入口
    void * user_param;//中断服务函数的参数
}sys_irq_handle_t;

void int_init(void);
void sys_irqtable_init(void);
void sys_register_irqhandle(IRQn_Type irq, system_irq_handler_t irq_handle, void * user_param);
void sys_irqhandle(u32 giccIar);
void default_irqhandle(u32 giccIar, void * user_param);

#endif // !__BSP_INT_H
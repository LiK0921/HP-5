#include "bsp_int.h"

//中断嵌套计数器
static u32 irq_Nesting = 0;
//制造中断向量表
static sys_irq_handle_t irqTable[NUMBER_OF_INT_VECTORS];



void int_init(void)
{
    //通用中断控制器初始化
    GIC_Init();
    //中断向量表初始化
    sys_irqtable_init();
    //ｃ语言版设置中断向量偏移到0x87800000
    __set_VBAR((u32)0x87800000);
}

//中断向量表初始化
void sys_irqtable_init(void)
{
    u16 i = 0;
    //嵌套清零
    irq_Nesting = 0;
    //整张向量表都初始化成默认中断服务函数和NULL参数
    for (i = 0; i < NUMBER_OF_INT_VECTORS; i++)
    {
        //注册中断服务函数
        sys_register_irqhandle(i, default_irqhandle, NULL); 
    }
}

//注册中断服务函数
void sys_register_irqhandle(IRQn_Type irq, system_irq_handler_t irq_handle, void * user_param)
{
    irqTable[irq].irq_handle = irq_handle;
    irqTable[irq].user_param = user_param;
}

//在汇编代码中，发生IRQ中断时调用～
void sys_irqhandle(u32 giccIar)
{
    u32 intNum = giccIar;
    if ((intNum == 0x3ff) || (intNum >= NUMBER_OF_INT_VECTORS))
    {
        return;
    }
    //嵌套加一
    irq_Nesting++;
    //调用相应的中断服务函数
    irqTable[intNum].irq_handle(intNum, irqTable[intNum].user_param);
    //嵌套减一
    irq_Nesting--;
}
//成默认中断服务函数
void default_irqhandle(u32 giccIar, void * user_param)
{
    while(1)
    {

    }
}


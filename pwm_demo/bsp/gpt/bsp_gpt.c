#include "bsp_gpt.h"
#include "bsp_int.h"
#include "bsp_led.h"
void gpt_init(GPT_Type * base, u32 irq_ms)
{
    base->CR = 0;
    base->CR = 1 << 15;//软复位
    while ((base->CR >> 15) & 0x01)
    {
        //等待复位完成
    }
    /*
        * GPT 的 CR 寄存器,GPT 通用设置
        * bit22:20 000 输出比较 1 的输出功能关闭，也就是对应的引脚没反应
        * bit9: 0 Restart 模式,当 CNT 等于 OCR1 的时候就产生中断
        * bit8:6 001 GPT 时钟源选择 ipg_clk=66Mhz
    */
    base->CR |= 1 << 6;//时钟源选择
    /*
    * GPT 的 PR 寄存器，GPT 的分频设置
    * bit11:0 设置分频值 0 ~ 4095
    */
    base->PR = 66 - 1;//分频系数设置

    /*
    * GPT 的 OCR1 寄存器，GPT 的输出比较 1 比较计数值，
    * GPT 的时钟为 1Mz，那么计数器每计一个值就是就是 1us。
    * 为了实现较大的计数，我们将比较值设置为最大的 0XFFFFFFFF,
    * 这样一次计满就是：0XFFFFFFFFus = 4294967296us = 4295s = 71.5min
    * 也就是说一次计满最多 71.5 分钟，存在溢出。
    */
    base->OCR[0] = 0xffffffff;//比较值设置
    
#if GPT1_IRQ_DISABLE
    /*
    * GPT 的 PR 寄存器，GPT 的分频设置
    * bit11:0 设置分频值，设置为 0 表示 1 分频，
    * 以此类推，最大可以设置为 0XFFF，也就是最大 4096 分频
    */
    base->PR = 66 - 1;//分频系数设置
    base->OCR[0] = irq_ms * 1000;//比较值设置，计时500ms就进入中断
    base->IR |= 1 << 0;//使能该定时器的通道１的比较中断
    //使能GPT1中断
    GIC_EnableIRQ(GPT1_IRQn);
    sys_register_irqhandle(GPT1_IRQn, (system_irq_handler_t)gpt1_irqhandle, NULL);
#endif

#if (GPT2_IRQ_ENABLE)
    if (base == GPT2)
    {
        /*
        * GPT 的 PR 寄存器，GPT 的分频设置
        * bit11:0 设置分频值，设置为 0 表示 1 分频，
        * 以此类推，最大可以设置为 0XFFF，也就是最大 4096 分频
        */
        GPT2->PR = 66 - 1;//分频系数设置
        GPT2->OCR[0] = irq_ms * 1000;//比较值设置，计时500ms就进入中断
        GPT2->IR |= 1 << 0;//使能该定时器的通道１的比较中断

        //先注册！！
        sys_register_irqhandle(GPT2_IRQn, (system_irq_handler_t)gpt2_irqhandle, NULL);
        //再使能GPT２中断
        GIC_EnableIRQ(GPT2_IRQn);
    } 
#endif
    base->CR |= 1 << 0;//使能定时器GPT
}

void gpt_delay_us(GPT_Type * base, u32 n)
{
    u64 oldcnt, newcnt;
    u64 tcntvalue = 0;

    oldcnt = base->CNT;//获取当前值
    while(1)
    {
        newcnt = base->CNT;//获取当前值
        if (newcnt != oldcnt)
        {
            if (newcnt > oldcnt)//向上计数，存在溢出的可能
            {
                tcntvalue += newcnt - oldcnt;//更新延时时间
            }
            else//已经溢出
            {
                
                //tcntvalue += 0xffffffff + newcnt - oldcnt;
                tcntvalue += base->OCR[0] + newcnt - oldcnt;
            }
            
            if (tcntvalue >= n)//延时时间大于形参是可以退出死循环了
            {
                break;
            }     
            oldcnt = newcnt;//保存历史时间戳
        }     
    }
}
void gpt_delay_ms(GPT_Type * base, u32 n)
{
    while (n--)
    {
        gpt_delay_us(base, 1000);
    }
}

#if GPT1_IRQ_DISABLE
void gpt1_irqhandle(u32 giccIar, void * user_param)
{
    static u8 state = OFF;
    
    if (GPT1->SR & (1 << 0))//500ms到了
    {      
        state = !state;
        led_switch(LED0, state);         
    }
    GPT1->SR |= (1 << 0);//写１清除中断标志位
}
#endif



#if GPT2_IRQ_ENABLE
void gpt2_irqhandle(u32 giccIar, void * user_param)
{
    static u8 state = OFF;
    
    if (GPT2->SR & (1 << 0))//500ms到了
    {      
        state = !state;
        led_switch(LED0, state);         
    }
    GPT2->SR |= (1 << 0);//写１清除中断标志位
}
#endif
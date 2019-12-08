#include "bsp_gpt.h"
#include "bsp_int.h"
#include "bsp_led.h"
void gpt_delay_init(void)
{
    GPT1->CR = 0;
    GPT1->CR = 1 << 15;//软复位
    while ((GPT1->CR >> 15) & 0x01)
    {
        //等待复位完成
    }
    /*
        * GPT 的 CR 寄存器,GPT 通用设置
        * bit22:20 000 输出比较 1 的输出功能关闭，也就是对应的引脚没反应
        * bit9: 0 Restart 模式,当 CNT 等于 OCR1 的时候就产生中断
        * bit8:6 001 GPT 时钟源选择 ipg_clk=66Mhz
    */
    GPT1->CR |= 1 << 6;//时钟源选择
    /*
    * GPT 的 PR 寄存器，GPT 的分频设置
    * bit11:0 设置分频值 0 ~ 4095
    */
    GPT1->PR = 66 - 1;//分频系数设置

    /*
    * GPT 的 OCR1 寄存器，GPT 的输出比较 1 比较计数值，
    * GPT 的时钟为 1Mz，那么计数器每计一个值就是就是 1us。
    * 为了实现较大的计数，我们将比较值设置为最大的 0XFFFFFFFF,
    * 这样一次计满就是：0XFFFFFFFFus = 4294967296us = 4295s = 71.5min
    * 也就是说一次计满最多 71.5 分钟，存在溢出。
    */
    GPT1->OCR[0] = 0xffffffff;//比较值设置
    GPT1->CR |= 1 << 0;//使能定时器GPT1

#if 1
    /*
    * GPT 的 PR 寄存器，GPT 的分频设置
    * bit11:0 设置分频值，设置为 0 表示 1 分频，
    * 以此类推，最大可以设置为 0XFFF，也就是最大 4096 分频
    */
    GPT1->PR = 66 - 1;//分频系数设置
    GPT1->OCR[0] = 500000;//比较值设置，计时500ms就进入中断
    GPT1->IR |= 1 << 0;//使能该定时器的通道１的比较中断

    //使能GPT1中断
    GIC_EnableIRQ(GPT1_IRQn);
    sys_register_irqhandle(GPT1_IRQn, gpt1_irqhandle, NULL);
#endif

}
void gpt_delay_us(u32 n)
{
    u64 oldcnt, newcnt;
    u64 tcntvalue = 0;

    oldcnt = GPT1->CNT;//获取当前值
    while(1)
    {
        newcnt = GPT1->CNT;//获取当前值
        if (newcnt != oldcnt)
        {
            if (newcnt > oldcnt)//向上计数，存在溢出的可能
            {
                tcntvalue += newcnt - oldcnt;//更新延时时间
            }
            else//已经溢出
            {
                tcntvalue += 0xffffffff + newcnt - oldcnt;
            }
            
            if (tcntvalue >= n)//延时时间大于形参是可以退出死循环了
            {
                break;
            }     
            oldcnt = newcnt;//保存历史时间戳
        }     
    }
}
void gpt_delay_ms(u32 n)
{
    while (n--)
    {
        gpt_delay_us(1000);
    }
}
#if 1
void gpt1_irqhandle(void)
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
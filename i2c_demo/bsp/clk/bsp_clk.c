#include "bsp_clk.h"

/*
    使能芯片内所有外设的时钟
*/
void clk_enble(void)
{
    CCM->CCGR0 = 0xffffffff;
    CCM->CCGR1 = 0xffffffff;
    CCM->CCGR2 = 0xffffffff;
    CCM->CCGR3 = 0xffffffff;
    CCM->CCGR4 = 0xffffffff;
    CCM->CCGR5 = 0xffffffff;
    CCM->CCGR6 = 0xffffffff;
}

/*
    时钟树的配置，配置成官方推荐的时钟频率
*/
void im6ull_clk_init(void)
{
    u32 reg = 0;

    if (((CCM->CCSR >> 2) & 0x1) == 0)//如果是ＰＬＬ，则切换到２４ＭＨｚ晶振作为ＡＲＭ内核时钟
    {
        CCM->CCSR &= ~(1 << 8);//选择tep_clk时钟源为24MH OSC
        CCM->CCSR |= (1 << 2);//配置pll1_sw_clk时钟源为step_clk
    }
    /* 设置pll1_main_clk为1056MHz,也就是528*2=1056MHZ,
    *      因为pll1_sw_clk进ARM内核的时候会被二分频！
    *      配置CCM_ANLOG->PLL_ARM寄存器
    *      bit13: 1 使能时钟输出
    *      bit[6:0]: 88, 由公式：Fout = Fin * div_select / 2.0，1056=24*div_select/2.0,
    *              		得出：div_select= 88  
    */
    //ARM PLL
	CCM_ANALOG->PLL_ARM = (1 << 13) | ((88 << 0) & 0X7F); 	/* 配置pll1_main_clk=1056MHz */
    CCM->CACRR = 1;		    /* ARM内核时钟为pll1_sw_clk二分频，528Mhz */
	CCM->CCSR &= ~(1 << 2);	/* 将pll_sw_clk时钟重新切换回pll1_main_clk */

	//超频６９６ＭＨＺ
	//CCM_ANALOG->PLL_ARM = (1 << 13) | ((58 << 0) & 0X7F); 	/* 配置pll1_main_clk=696MHz */
    //CCM->CACRR = 0;		    /* ARM内核时钟为pll1_sw_clk不分频，696Mhz */
	//CCM->CCSR &= ~(1 << 2);	/* 将pll_sw_clk时钟重新切换回pll1_main_clk */

	
	//设置PLL2(SYS PLL)各个PFD
	reg = CCM_ANALOG->PFD_528;
	reg &= ~(0X3F3F3F3F);		//清除原来的设置 					
	reg |= 32<<24;				// PLL2_PFD3=528*18/32=297Mhz 
	reg |= 24<<16;				// PLL2_PFD2=528*18/24=396Mhz(DDR使用的时钟，最大400Mhz) 
	reg |= 16<<8;				// PLL2_PFD1=528*18/16=594Mhz 
	reg |= 27<<0;				// PLL2_PFD0=528*18/27=352Mhz  	
	CCM_ANALOG->PFD_528 = reg;	// 设置PLL2_PFD0~3 		 	

	//设置PLL3(USB1 PLL)各个PFD
	reg = 0;					// 清零  
	reg = CCM_ANALOG->PFD_480;
	reg &= ~(0X3F3F3F3F);		// 清除原来的设置 						
	reg |= 19<<24;				// PLL3_PFD3=480*18/19=454.74Mhz 	
	reg |= 17<<16;				// PLL3_PFD2=480*18/17=508.24Mhz 	
	reg |= 16<<8;				// PLL3_PFD1=480*18/16=540Mhz		
	reg |= 12<<0;				// PLL3_PFD0=480*18/12=720Mhz	 	
	CCM_ANALOG->PFD_480 = reg;	// 设置PLL3_PFD0~3 					

	//设置AHB时钟 最小6Mhz， 最大132Mhz (boot rom自动设置好了可以不用设置)*/
	CCM->CBCMR &= ~(3 << 18); 	/* 清除设置*/ 
	CCM->CBCMR |= (1 << 18);	/* pre_periph_clk=PLL2_PFD2=396MHz */
	CCM->CBCDR &= ~(1 << 25);	/* periph_clk=pre_periph_clk=396MHz */

	while(CCM->CDHIPR & (1 << 5));/* 等待配置完成 */
		
	/* 修改AHB_PODF位的时候需要先禁止AHB_CLK_ROOT的输出，但是
	 * 我没有找到关闭AHB_CLK_ROOT输出的的寄存器，所以就没法设置。
	 * 下面设置AHB_PODF的代码仅供学习参考不能直接拿来使用！！
	 * 内部boot rom将AHB_PODF设置为了3分频，即使我们不设置AHB_PODF，
	 * AHB_ROOT_CLK也依旧等于396/3=132Mhz。
	 */
#if 0
	/* 要先关闭AHB_ROOT_CLK输出，否则时钟设置会出错 */
	CCM->CBCDR &= ~(7 << 10);	/* CBCDR的AHB_PODF清零 */
	CCM->CBCDR |= 2 << 10;		/* AHB_PODF 3分频，AHB_CLK_ROOT=132MHz */
	while(CCM->CDHIPR & (1 << 1));/
* 等待握手完成 */
#endif
	
	//以下两个时钟根ROOT,会有很多外设需要用到，所以先把大路修好，以后具体哪个外设再修小路。

	//设置IPG_CLK_ROOT最小3Mhz，最大66Mhz (boot rom自动设置好了可以不用设置)
	CCM->CBCDR &= ~(3 << 8);	/* CBCDR的IPG_PODF清零 */
	CCM->CBCDR |= 1 << 8;		/* IPG_PODF 2分频，IPG_CLK_ROOT=66MHz */
	
	//设置PERCLK_CLK_ROOT时钟
	CCM->CSCMR1 &= ~(1 << 6);	/* PERCLK_CLK_ROOT时钟源为IPG */
	CCM->CSCMR1 &= ~(7 << 0);	/* PERCLK_PODF位清零，即1分频 */


	//配置串口的时钟
	CCM->CSCDR1 &= ~(1 << 6);//80MHz

	CCM->CSCDR1 &= ~(0x3F);//1分频

}
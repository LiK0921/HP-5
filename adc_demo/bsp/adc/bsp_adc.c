#include "bsp_adc.h"
#include "bsp_gpio.h"

void adc_init(ADC_Type * base)
{
    gpio_pin_config_t gpio_config;
    gpio_config.direction = kGPIO_DigitalInput;
    gpio_config.interrupt_mode = kGPIO_NoIntmode;
    
    if (base == ADC1)
    {

        //初始化为复用pwm1
        IOMUXC_SetPinMux(IOMUXC_GPIO1_IO01_GPIO1_IO01, 1);
        //设置电气属性
        IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO01_GPIO1_IO01, 0x6080);
        //gpio初始化
        gpio_init(GPIO1, 1, &gpio_config);
    }
    //配置寄存器
    base->CFG = 0;
    //            时钟33Mhz  12位ADC    长采样      不分频      8次平均　   允许覆盖上一次未读的值
    base->CFG |= (1 << 0) | (2 << 2) | (1 << 4) | (0 << 5) | (1 << 14) | (1 << 16);

    //通用控制，比如校准，连续转换，硬件平均，软硬触发选择，比较功能，参考电压选择
    base->GC = 0;
    //硬件取8次平均 不用DMA 不开启比较范围  禁止比较功能  不连续转换
    base->GC |= (1 << 5) | (0 << 0) | (0 << 2) | (0 << 4) | (0 << 6);

    //等待校验完成
    while (adc_calibration(base))
    {
        
    }
    //设为软件触发
    base->CFG &= ~(1 << 13);
}

//float voltage_f = 0;
u16 voltage_i = 0;
void adc_read(ADC_Type * base)
{
    u16 adc_val = 0;
    base->HC[0] = 1;//软件启动触发

    //等待采样完成
    while (base->HS & 0x1)
    {
        /* code */
    }
    adc_val = base->R[0];//获取原始数据
    //voltage_f = adc_val * 3.30f / 4096.0f;
    //voltage_i = (u16)(voltage_f * 100);

    voltage_i = adc_val * 330  / 4096;//放大100倍，为了显示小数在LCD上
}

u8 adc_calibration(ADC_Type * base)
{
    u8 cali_status = 0;
    u16 cali_timeout = 0xffff;
    base->CFG &= ~(1 << 13);//校验前必须设置软件触发模式。

    //通用状态寄存器。GS的bit[0]为0则表示转换完成或终止，1则正在转换
    base->GS |= (1 << 1);//清空CALF位，为1则表示校准失败
    //控制寄存器的bit[7],置1校准
    base->GC |= (1 << 7);//启动校准

    while (base->GC & (1 << 7) && (cali_timeout > 0))//校准过程中CAL位保持1,结束自动清零
    {
        //校准过程失败则CALF位会拉高
        if (base->GS & (1 << 1))
        {
            cali_status = 1;//校准失败
        }
        cali_timeout--;
    }

    //校准完成后检查coco0位是否拉高
    if (((base->HS & (1 << 0)) == 0) || (base->GS & (1 << 1)))
    {
        cali_status = 1;//校准失败
    }
  
    base->R[0];//读取一次用来清空coco0位
    ADC1->CFG |= (1 << 13);  //恢复硬件触发模式

    return cali_status;
}

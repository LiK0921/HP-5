#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#include "imx6ul.h"

extern u16 voltage_i;

void adc_init(ADC_Type * base);

void adc_read(ADC_Type * base);

u8 adc_calibration(ADC_Type * base);

#endif // !__BSP_ADC_H
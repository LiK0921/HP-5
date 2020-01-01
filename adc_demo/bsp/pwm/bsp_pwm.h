#ifndef __BSP_PWM_H
#define __BSP_PWM_H

#include "imx6ul.h"

#define PWM1_ENABLE     1
#define PWM2_ENABLE     1
#define PWM3_ENABLE     1
#define PWM4_ENABLE     1

#define PWM1_DISABLE      0
#define PWM2_DISABLE      0
#define PWM3_DISABLE      0
#define PWM4_DISABLE      0



void pwm_init(PWM_Type * base, u32 hz);
void pwm_enable(PWM_Type * base);
void pwm_disable(PWM_Type * base);
void pwm_set_period(PWM_Type * base, u32 hz);
void pwm_set_duty(PWM_Type * base, u8 duty);
void pwm_fifo_int_enable(PWM_Type * base);



void pwm1_irqhandle(u32 giccIar, void * user_param);

#endif // !__BSP_PWM_H

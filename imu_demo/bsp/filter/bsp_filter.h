#ifndef __BSP_FILTER_H
#define __BSP_FILTER_H

#include "imx6ul.h"

#define     K1  0.05f
//#define     dt  0.005f;//卡尔曼滤波采样时间参量，5ms，陀螺仪中断中执行，应调节到使滤波平滑，跟随快滞后少

void Kalman_Filter(float Accel_Angle[2],float Gyro[2], float dt);		
void Yijie_Filter(float angle_m[2], float gyro_m[2], float dt);

#endif // !__BSP_FILTER_H
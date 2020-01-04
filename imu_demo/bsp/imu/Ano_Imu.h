#ifndef __ANO_IMU_H
#define __ANO_IMU_H
#include "Ano_Math.h"
#include "imx6ul.h"


typedef struct
{
	float w;//q0;
	float x;//q1;
	float y;//q2;
	float z;//q3;
	float x_vec[3];
	float y_vec[3];
	float z_vec[3];//用来装四元数矩阵第三行元素的，就是重力分量

	float rol;
	float pit;
	float yaw;
}imu_st;

extern imu_st imu;

void IMU_update(float dT, float gyr[3], float acc[3],imu_st *imu);
void calculate_RPY(void);
#endif


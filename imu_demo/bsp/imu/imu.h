#ifndef _IMU_H_
#define	_IMU_H_

#include "imx6ul.h"
#include "Ano_Math.h"

enum
{
	x = 0,
	y = 1, 
	z = 2
};

typedef struct
{
	float x;
	float y;
	float z;
}xyz_f_t;

typedef struct 
{
	xyz_f_t err;
	xyz_f_t err_tmp;
	xyz_f_t err_lpf;
	xyz_f_t err_Int;
	xyz_f_t g;
}ref_t;

extern xyz_f_t reference_v;

extern float Pitch, Roll, Yaw; 
extern float Accel[3],Gyro[3];

void update(float half_T,float gx, float gy, float gz, float ax, float ay, float az,float *rol, float *pit,float *yaw);


#endif


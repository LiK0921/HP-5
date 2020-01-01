#ifndef _IMU_H_
#define	_IMU_H_

#include "imx6ul.h"
#include "math.h"

#define LIMIT( x,min,max ) ( ((x) <= (min)) ? (min) : ( ((x) > (max))? (max) : (x) ) )

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
extern float Accel_Y, Accel_X, Accel_Z, Gyro_X, Gyro_Y, Gyro_Z;

void update(float half_T,float gx, float gy, float gz, float ax, float ay, float az,float *rol, float *pit,float *yaw);

float my_sqrt(float number);
float my_sqrt_reciprocal(float number);

#endif


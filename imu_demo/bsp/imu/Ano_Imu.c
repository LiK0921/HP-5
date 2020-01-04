#include "Ano_Imu.h"

static float vec_err[3];
static float vec_err_i[3];

static float vec_err_temp[3];
					 
static float q0q1,q0q2,q1q1,q1q3,q2q2,q2q3,q3q3,q1q2,q0q3;

//拓空者中
imu_st imu = {1,0,0,0,{0,0,0},{0,0,0},{0,0,0}};
				
#define REF_ERR_LPF_HZ  	1	//(Hz)

void IMU_update(float dT, float gyr[3], float acc[3], imu_st * imu)
{
	u8 i = 0;	
	float 	ref_err_lpf_hz;//误差低通
								
	static float kp_use = 1.1f, ki_use = 0.001f;//积分项要调小，不然会震荡，也可设为0
	
	float acc_norm_l;
	float q_norm_l;
	
	float acc_norm[3];

	float d_angle[3];
						
	q0q1 = imu->w * imu->x;
	q0q2 = imu->w * imu->y;
	q1q1 = imu->x * imu->x;
	q1q3 = imu->x * imu->z;
	q2q2 = imu->y * imu->y;
	q2q3 = imu->y * imu->z;
	q3q3 = imu->z * imu->z;
	q1q2 = imu->x * imu->y;
	q0q3 = imu->w * imu->z;
					
	acc_norm_l = my_sqrt_reciprocal(acc[0] * acc[0] + acc[1] * acc[1] + acc[2] * acc[2]);	
		
	// 加速度计的读数，单位化。
	for(i = 0;i<3;i++)
	{
		acc_norm[i] = acc[i] * acc_norm_l;
	}

	// 载体坐标下的x方向向量，单位化。
    imu->x_vec[0] = 1 - (2*q2q2 + 2*q3q3);
    imu->x_vec[1] = 2*q1q2 - 2*q0q3;
    imu->x_vec[2] = 2*q1q3 + 2*q0q2;
		
	// 载体坐标下的y方向向量，单位化。
    imu->y_vec[0] = 2*q1q2 + 2*q0q3;
    imu->y_vec[1] = 1 - (2*q1q1 + 2*q3q3);
    imu->y_vec[2] = 2*q2q3 - 2*q0q1;

    //载体坐标下的z方向向量（等效重力向量、重力加速度向量），单位化。
    imu->z_vec[0] = 2*q1q3 - 2*q0q2;
    imu->z_vec[1] = 2*q2q3 + 2*q0q1;
    imu->z_vec[2] = 1 - (2*q1q1 + 2*q2q2);
	
    // 测量值与等效重力向量的叉积（计算向量误差）。
    vec_err_temp[0] =  (acc_norm[1] * imu->z_vec[2] - imu->z_vec[1] * acc_norm[2]);
    vec_err_temp[1] = -(acc_norm[0] * imu->z_vec[2] - imu->z_vec[0] * acc_norm[2]);
    vec_err_temp[2] = -(acc_norm[1] * imu->z_vec[0] - imu->z_vec[1] * acc_norm[0]);

	/* 误差低通 */
	ref_err_lpf_hz = REF_ERR_LPF_HZ *(6.28f * dT * 0.5f);
	for(i = 0;i<3;i++)
	{
		vec_err[i] += ref_err_lpf_hz * (vec_err_temp[i] - vec_err[i]);
	}
	
	for(i = 0;i<3;i++)
	{
		//误差积分
		vec_err_i[i] += vec_err[i] * dT * ki_use;
		vec_err_i[i] = LIMIT(vec_err_i[i] , -0.02f, 0.02f);
				
		d_angle[i] = (gyr[i] + (vec_err[i] + vec_err_i[i]) * kp_use) * dT * 0.5f;
	}
    // 龙格库塔更新四元数，计算姿态。
    imu->w = imu->w            - imu->x*d_angle[0] - imu->y*d_angle[1] - imu->z*d_angle[2];
    imu->x = imu->w*d_angle[0] + imu->x            + imu->y*d_angle[2] - imu->z*d_angle[1];
    imu->y = imu->w*d_angle[1] - imu->x*d_angle[2] + imu->y            + imu->z*d_angle[0];
    imu->z = imu->w*d_angle[2] + imu->x*d_angle[1] - imu->y*d_angle[0] + imu->z;
		
	q_norm_l = my_sqrt_reciprocal(imu->w*imu->w + imu->x*imu->x + imu->y*imu->y + imu->z*imu->z);
    //四元数归一化
	imu->w *= q_norm_l;
    imu->x *= q_norm_l;
    imu->y *= q_norm_l;
    imu->z *= q_norm_l;
	
	//转换为欧拉角
	calculate_RPY();
}

static float t_temp;
void calculate_RPY()
{
	///////////////////////输出姿态角///////////////////////////////	
	t_temp = LIMIT(1 - my_pow(imu.z_vec[0]),0,1);
	
	if(ABS(imu.z_vec[2])>0.05f)//避免奇点的运算
	{
		imu.pit =  fast_atan2(imu.z_vec[0],my_sqrt(t_temp))*57.30f;
		imu.rol =  fast_atan2(imu.z_vec[1], imu.z_vec[2])*57.30f; 
		imu.yaw = -fast_atan2(imu.y_vec[0], imu.x_vec[0])*57.30f; 
	}
}




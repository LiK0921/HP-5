#include "bsp_filter.h"


extern float    Filter_Angle[3];

float Q_angle = 0.001f;// 过程噪声的协方差

float Q_gyro  = 0.003f;//0.003 过程噪声的协方差 过程噪声的协方差为一个一行两列矩阵

float R_angle = 0.5f;//角度测量噪声值  测量噪声的协方差 既测量偏差

float Q_bias[2], Angle_err[2];

float PCt_0, PCt_1, E;//计算的过程量

float K_0, K_1, t_0, t_1;

float Pdot[4] = {0,0,0,0};//过程协方差矩阵的微分矩阵
float PP[2][2] = { { 1, 0 },{ 0, 1 } };//协方差

		//由加速度计计算出的角度值	//陀螺仪测得的角速度值
void Kalman_Filter(float Accel_Angle[2],float Gyro[2], float dt)		
{
	u8 i = 0;
    for ( i = 0; i < 2; i++)
    {
        Filter_Angle[i] += (Gyro[i] - Q_bias[i]) * dt; //陀螺仪积分角度（先验估计）
        //根据角度等于时间的微分乘以角速度，k时刻角度可近似认为是k-1时刻的角度值
        //加上k-1时刻陀螺仪测得的角速度值乘以时间，但先要减去陀螺仪的静态漂移。
        
        PP[0][0] += ((- PP[0][1] - PP[1][0]) * dt + Q_angle);// Pk-先验估计误差协方差微分的积分
        PP[0][1] += (- PP[1][1] * dt);   // =先验估计误差协方差
        PP[1][0] += (- PP[1][1] * dt);
        PP[1][1] += Q_gyro ;
 
        
        //计算角度偏差
        Angle_err[i] = Accel_Angle[i] - Filter_Angle[i];	//zk-先验估计
        
        //计算卡尔曼增益矩阵。两行一列   K_0，K_1  
        E = R_angle + PP[0][0];
        K_0 = PP[0][0] / E;       
        K_1 = PP[1][0] / E;
        
        //对矩阵PP进行更新
        //P(k|k)=（I-Kg(k) H）P(k|k-1) //更新协方差阵
        PP[0][0] -= K_0 * PP[0][0];		 //后验估计误差协方差
        PP[0][1] -= K_0 * PP[0][1];
        PP[1][0] -= K_1 * PP[0][0];
        PP[1][1] -= K_1 * PP[0][1];
 
        //X(k|k)= X(k|k-1)+Kg(k) (Z(k) - H X(k|k-1)) //通过卡尔曼增益进行修正     
        Filter_Angle[i]	+= K_0 * Angle_err[i];	 //后验估计最优角度值     
        Q_bias[i]	+= K_1 * Angle_err[i];	 //后验估计最优估计值的偏差
    }
}

void Yijie_Filter(float angle_m[2], float gyro_m[2], float dt)
{
    u8 i = 0;
    for (i = 0; i < 2; i++)
    {
        Filter_Angle[i] = K1 * angle_m[i] + (1 - K1) * (Filter_Angle[i] + gyro_m[i] * dt);
    }
}
#include "imu.h"


#define Kp 1.1f   // proportional gain governs rate of convergence to accelerometer/magnetometer
#define Ki 0.1f   // 0.001  integral gain governs rate of convergence of gyroscope biases

#define REF_ERR_LPF_HZ  	1	//(Hz)

xyz_f_t 	reference_v;

ref_t 		ref;

float ref_q[4] = {1,0,0,0};

float norm_acc, norm_q;

float temp = 0;

//				  半周期			//三轴弧度制角速度				//三轴加速度				//横滚角
void update(float half_T,float gx, float gy, float gz, float ax, float ay, float az,float *rol, float *pit,float *yaw) 
{		
	float 	ref_err_lpf_hz;
	
	// 计算等效重力向量
	reference_v.x = 2*(ref_q[1]*ref_q[3] - ref_q[0]*ref_q[2]);
	reference_v.y = 2*(ref_q[0]*ref_q[1] + ref_q[2]*ref_q[3]);
	reference_v.z = 1 - 2*(ref_q[1]*ref_q[1] + ref_q[2]*ref_q[2]);

	//计算加速度向量的模  根号分之一
	norm_acc = my_sqrt_reciprocal(ax * ax + ay * ay + az * az);   

	//把加计的三维向量转成单位向量。
	ax *= norm_acc;//4096.0f;
	ay *= norm_acc;//4096.0f;
	az *= norm_acc;//4096.0f; 
		

	/* 叉乘得到误差 */
	ref.err_tmp.x = ay * reference_v.z - az * reference_v.y;
	ref.err_tmp.y = az * reference_v.x - ax * reference_v.z;
	ref.err_tmp.z = ax * reference_v.y - ay * reference_v.x;
		
	/* 误差低通 */
	ref_err_lpf_hz = REF_ERR_LPF_HZ *(6.28f * half_T);
	ref.err_lpf.x += ref_err_lpf_hz *( ref.err_tmp.x  - ref.err_lpf.x );
	ref.err_lpf.y += ref_err_lpf_hz *( ref.err_tmp.y  - ref.err_lpf.y );
	ref.err_lpf.z += ref_err_lpf_hz *( ref.err_tmp.z  - ref.err_lpf.z );
	
	ref.err.x = ref.err_lpf.x;
	ref.err.y = ref.err_lpf.y;
	ref.err.z = ref.err_lpf.z ;
	
	/* 误差积分 */
	ref.err_Int.x += ref.err.x * Ki * 2 * half_T ;
	ref.err_Int.y += ref.err.y * Ki * 2 * half_T ;
	ref.err_Int.z += ref.err.z * Ki * 2 * half_T ;
	
	/* 积分限幅 */
	ref.err_Int.x = LIMIT(ref.err_Int.x, - 0.02f ,0.02f );
	ref.err_Int.y = LIMIT(ref.err_Int.y, - 0.02f ,0.02f );
	ref.err_Int.z = LIMIT(ref.err_Int.z, - 0.02f ,0.02f );	

	ref.g.x = gx + ( Kp*(ref.err.x + ref.err_Int.x) ) ;     //IN RADIAN
	ref.g.y = gy + ( Kp*(ref.err.y + ref.err_Int.y) ) ;		//IN RADIAN
	ref.g.z = gz + ( Kp*(ref.err.z + ref.err_Int.z) ) ;
	
	/* 用叉积误差来做PI修正陀螺零偏 */
	//一阶龙格库塔法求解新的四元数
	ref_q[0] = ref_q[0] + (-ref_q[1]*ref.g.x - ref_q[2]*ref.g.y - ref_q[3]*ref.g.z)*half_T;
	ref_q[1] = ref_q[1] + (ref_q[0]*ref.g.x + ref_q[2]*ref.g.z - ref_q[3]*ref.g.y)*half_T;
	ref_q[2] = ref_q[2] + (ref_q[0]*ref.g.y - ref_q[1]*ref.g.z + ref_q[3]*ref.g.x)*half_T;
	ref_q[3] = ref_q[3] + (ref_q[0]*ref.g.z + ref_q[1]*ref.g.y - ref_q[2]*ref.g.x)*half_T;  

	/* 四元数规一化 */
	norm_q = my_sqrt_reciprocal(ref_q[0]*ref_q[0] + ref_q[1]*ref_q[1] + ref_q[2]*ref_q[2] + ref_q[3]*ref_q[3]);
	ref_q[0] *= norm_q;
	ref_q[1] *= norm_q;
	ref_q[2] *= norm_q;
	ref_q[3] *= norm_q;
	

	temp = LIMIT(1 - my_pow(2*(ref_q[1]*ref_q[3] - ref_q[0]*ref_q[2])),0,1);
	//用归一化的四元数换算成欧拉角用于PID控制！！！
	*rol = fast_atan2(2*(ref_q[0]*ref_q[1] + ref_q[2]*ref_q[3]),1 - 2*(ref_q[1]*ref_q[1] + ref_q[2]*ref_q[2])) *57.3f;
	*pit = fast_atan2(2*(ref_q[1]*ref_q[3] - ref_q[0]*ref_q[2]), my_sqrt(LIMIT(1 - my_pow(temp),0,1))) *57.3f;
	*yaw = -fast_atan2(2*(ref_q[1]*ref_q[2] + ref_q[0]*ref_q[3]), 1 - 2*(ref_q[2]*ref_q[2] + ref_q[3]*ref_q[3])) *57.3f;
}



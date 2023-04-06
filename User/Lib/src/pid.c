#include "pid.h"

/**
 * @brief		所有pid初始化
 * @author		whale
 * @param [in]	pid参数
 * @retval		void
  */
// static void pid_init(Pid_Position_t*pid,float kp,float ki,float kd,float limit_i,float limit_out,float is_i_spare,float begin_i,float stop_grow_i)
// {
// 	pid->kp = kp;
// 	pid->ki = ki;
// 	pid->kd = kd;
// 	pid->tar = 0;
// 	pid->cur = 0;
// 	pid->err = 0;
// 	pid->old_err = 0;
// 	pid->is_integral_spare = is_i_spare;
// 	pid->begin_integral = begin_i;
// 	pid->stop_grow_integral = stop_grow_i;
// 	pid->p_out = 0;
// 	pid->i_out = 0;
// 	pid->d_out = 0;
// 	pid->output = 0;
// 	pid->max_err_integral = limit_i;
// 	pid->max_out = limit_out;
// }
// void All_Pid_Configuration(PID pid[])
/* v0.1:原格式本来是这样，报错：存在正文时不允许未命名的原型参数。查询后得知由于C语言不能使用引用的原因，所以改成指针 */
/* V0.2:argument of type "Pid_Position_t *" is incompatible with parameter of type "struct Pid_Position_t *" */
/* V0.3:发现了形参命名不一致的错误但是还是报错 */
/* V0.4:使用强制类型转换把形参从Pid_Position_t*转换为struact Pid_Parameter* */
/* V0.5:发现形参命名空间不一样，修改结构体定义，报错消息，类型的警告还在 */
/* V0.6：注释了用于初始化结构体函数指针，算是完成了 */
/* 	v0.7:不使用函数指针 */

/**
 * @brief		PID参数初始化
 * @note		暂时未使用，还未替代原来的参数以及初始化，因为使用new的办法初始化结构体的时候报错
*/
Pid_Position_t pid_parameter[Chassis_Follow_Pid];//初始化
	//can1_device.c中
	//can2_device.c中
/**
 * @brief			pid参数重置
 * @param [in]		pid
 * @retval			void
 * @note			待完善
 */
// void pid_reset(Pid_Position_t *pid)
// {
// 	pid->kp = 0;
// 	pid->ki = 0;
// 	pid->kd = 0;
// 	pid->p_out = 0;
// 	pid->i_out = 0;
// 	pid->d_out = 0;
// 	pid->output = 0;
// }
/**
 * @brief			pid value limit
 * @param[in]		pid Pid_Position_t struct
 * @retval			none
 */
static float Pid_Limit(float value, float min, float max)
{
	if (value < min)
		return min;
	if (value > max)
		return max;

	return value;
}

/**
 * @brief			calculate position pid,statice method,call in Pid_Position_Calc()
 * @param[in]		pid Pid_Position_t struct
 * @retval			none
 */
static void Calc_Position_Pid(Pid_Position_t *pid)
{
	float intergal_spare_k = 1.0f;

	pid->err = pid->tar - pid->cur;
	/* if use integral spare pid, default is not used */
	if (pid->is_integral_spare)
	{
		if (fabs(pid->err) > pid->begin_integral)
		{
			intergal_spare_k = 0.0f;
		}
		else if (fabs(pid->err) < pid->stop_grow_integral)
		{
			pid->err_integral += pid->err;
			intergal_spare_k = 1.0f;
		}
		else
		{
			pid->err_integral += pid->err;
			intergal_spare_k = (pid->begin_integral - fabs(pid->err)) / (pid->begin_integral - pid->stop_grow_integral);
		}
	}
	else
	{
		pid->err_integral += pid->err;
	}

	pid->err_integral = Pid_Limit(pid->err_integral, -pid->max_err_integral, pid->max_err_integral);

	pid->p_out = pid->kp * pid->err;
	pid->i_out = pid->ki * pid->err_integral * intergal_spare_k;
	pid->d_out = pid->kd * (pid->err - pid->old_err);

	pid->output = pid->p_out + pid->i_out + pid->d_out;

	/* output limit */
	pid->output = Pid_Limit(pid->output, -pid->max_out, pid->max_out);

	pid->old_err = pid->err;
}

/**
 * @brief			calculate position pid
 * @param[in]		pid: Pid_Position_t struct, save pid param
 * @param[in]		tar: target value
 * @param[in]		cur: current value
 * @retval			pid->out: pid calculate result
 */
float Pid_Position_Calc(Pid_Position_t *pid, float tar, float cur)
{
	pid->tar = tar;
	pid->cur = cur;
	Calc_Position_Pid(pid);
	return pid->output;
}

/**
 * @brief			calculate increasement pid,static method
 * @param			pid Pid_Increment_t struct
 * @retval			none
 */
static void Calc_Increment_Pid(Pid_Increment_t *pid)
{
	pid->err = pid->tar - pid->cur;

	pid->p_out = pid->kp * (pid->err - pid->old_err);
	pid->i_out = pid->ki * pid->err;
	pid->d_out = pid->kd * (pid->err - 2 * pid->old_err + pid->old_old_err);

	pid->output = pid->p_out + pid->i_out + pid->d_out;

	/* output limit */
	pid->output = Pid_Limit(pid->output, -pid->max_out, pid->max_out);

	pid->old_old_err = pid->old_err;
	pid->old_err = pid->err;
}

/**
 * @brief			calculate increasement pid
 * @param[in]		pid: Pid_Increment_t struct, pid param
 * @param[in]		tar: target value
 * @param[in]		cur: current value
 * @retval			pid->out: pid calculate result
 */
float Pid_Increment_Calc(Pid_Increment_t *pid, float tar, float cur)
{
	pid->tar = tar;
	pid->cur = cur;
	Calc_Increment_Pid(pid);
	return pid->output;
}

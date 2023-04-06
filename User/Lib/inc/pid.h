#ifndef __PID_H
#define __PID_H

#include "stm32f4xx.h"
#include "math.h"
#include "motor.h"
// static Pid_Position_t motor_yaw_speed_pid = NEW_POSITION_PID(1800, 0.8, 0.2, 5000, 30000, 0, 1000, 500); //yaw电机速度PID
#define ALL_PID 11

// struct Pid_Position_t;///<为了解决结构体成员的函数指针依赖加了一行

#define NEW_POSITION_PID(p, i, d, limit_i, limit_out, is_i_spare, begin_i, stop_grow_i) \
	{                                                                                   \
		.kp = p,                                                                        \
		.ki = i,                                                                        \
		.kd = d,                                                                        \
		.tar = 0,                                                                       \
		.cur = 0,                                                                       \
		.err = 0,                                                                       \
		.old_err = 0,                                                                   \
		.err_integral = 0,                                                              \
		.is_integral_spare = is_i_spare,                                                \
		.begin_integral = begin_i,                                                      \
		.stop_grow_integral = stop_grow_i,                                              \
		.p_out = 0,                                                                     \
		.i_out = 0,                                                                     \
		.d_out = 0,                                                                     \
		.output = 0,                                                                    \
		.max_err_integral = limit_i,                                                    \
		.max_out = limit_out                                                            \
	}

#define NEW_INCREMENT_PID(p, i, d, limit) \
	{                                     \
		.kp = p,                          \
		.ki = i,                          \
		.kd = d,                          \
		.tar = 0,                         \
		.cur = 0,                         \
		.err = 0,                         \
		.old_err = 0,                     \
		.old_old_err = 0,                 \
		.p_out = 0,                       \
		.i_out = 0,                       \
		.d_out = 0,                       \
		.output = 0,                      \
		.max_out = limit                  \
	}

typedef struct
{
	float kp;
	float ki;
	float kd;

	float tar;
	float cur;

	float err;
	float old_err;
	float err_integral;
	uint8_t is_integral_spare;

	float begin_integral;
	float stop_grow_integral;

	float max_err_integral;
	float max_out;

	float p_out;
	float i_out;
	float d_out;
	float output;

	// void(*pid_init)( struct Pid_Position_t *pid, float kp, float ki, float kd, float poutmax, float ioutmax, float doutmax, float outmax);			//用来初始化pid
	// void(*pid_reset)(struct Pid_Position_t *pid);
	// /* 指向函数的指针。这里作为结构体成员
	//  * 两个函数分别是不同pid计算的初始化以及重置。
	//暂时不用函数指针
	//  */
} Pid_Position_t;

typedef struct
{

	float kp;
	float ki;
	float kd;

	float tar;
	float cur;

	float err;
	float old_err;
	float old_old_err;

	float max_out;

	float p_out;
	float i_out;
	float d_out;

	float output;

	float angle_output;

} Pid_Increment_t;
enum pid_name
{
	Chassis_Follow_Pid = 1,
	chassis_Speed_fl,
	Chassis_Speed_fr,
	Chassis_Speed_bl,
	Chassis_Speed_br,

	Yaw_target_Angle,
	Yaw_target_Speed,

	Pitch_target_Angle,
	Pitch_target_Speed,

	wave_Speed
};

///
float Pid_Position_Calc(Pid_Position_t *pid, float tar, float cur);
float Pid_Increment_Calc(Pid_Increment_t *pid, float tar, float cur);
void All_Pid_Config(Pid_Position_t[]);
// void pid_init(Pid_Position_t*pid,float kp,float ki,float kd,float limit_i,float limit_out,float is_i_spare,float begin_i,float stop_grow_i);
// void pid_reset(Pid_Position_t *pid);
void All_Pid_Config(Pid_Position_t[]);

#endif

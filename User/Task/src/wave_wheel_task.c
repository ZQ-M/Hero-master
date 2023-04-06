/**
 * @file wave_wheel_task.c
 * @brief 波轮电机任务，任务开始进入阻塞状态，等待 shoot_task 的唤醒
 * TODO: 待加入连发。改进 PID 计算的结束条件，在测试中出现了发射一次转多次的情况（英雄暂定不加入连发模式，全部单发）
 *
 * @version 0.1
 * @date 2021-10-17
 * @copyright Copyright (c) 2021
 */
#include "wave_wheel_task.h"

static Rc_Ctrl_t *rc_data_pt;
static Robot_control_data_t *robot_control_data_pt;
static int *angle_integral;
static Motor_Measure_t *wave_motor_feedback_data;
static int8_t *is_ok_fire;
static const int32_t *fire_one_bullet_signal;
static const int32_t *fire_five_bullet_signal;

static float wave_motor_speed = 0.0;
static const int32_t wave_once_machine_angle = -31488;

static void Emission_Once_Time(void);

void StartWaveWheelTask(void const *argument)
{
	osEvent fire_event; ///<等待摩擦轮就绪
	rc_data_pt = Get_Rc_Parsed_RemoteData_Pointer();
	wave_motor_feedback_data = Get_Wave_Motor_Paresed_Data();
	is_ok_fire = Get_Is_OK_Fire(); ///<摩擦轮设置速度就绪标志位
	fire_one_bullet_signal = Get_Fire_One_Bullet_Signal();
	fire_five_bullet_signal = Get_Fire_Five_Bullet_Signal();

	(void)rc_data_pt;
	(void)robot_control_data_pt;

	angle_integral = Get_Error_Integral();

	osDelay(100);

	for (;;)
	{
		fire_event = osSignalWait((*fire_one_bullet_signal) | (*fire_five_bullet_signal), osWaitForever); ///<等待射击任务唤醒。即摩擦轮转动后波轮才会转动
		if (fire_event.status == osEventSignal)
		{
			if ((fire_event.value.signals == *fire_one_bullet_signal) && *is_ok_fire == 1)
			{
				Emission_Once_Time();
			}
			else if ((fire_event.value.signals == *fire_five_bullet_signal) && *is_ok_fire == 1)
			{
			}
		}
		osDelay(10);
	}
}

static void Emission_Once_Time(void)
{
	uint32_t start_time = 0;
	uint32_t cal_pid_time = 0;
	uint32_t end_time = 0;
	wave_motor_speed = Calc_Wave_Motor_Angle8191_Pid(wave_once_machine_angle, *angle_integral);

	///< 初步测试，发射一颗弹丸的 pid 计算时间最多是 320 ms,时间在 290-320 之间徘徊
	while ((fabs(wave_motor_speed) > 10) && (*angle_integral > -31300))
	{
		start_time = osKernelSysTick();
		wave_motor_speed = Calc_Wave_Motor_Angle8191_Pid(wave_once_machine_angle, *angle_integral);
		Set_Wave_Motor_Speed(wave_motor_speed, wave_motor_feedback_data);
		osDelay(5);
		///< 堵转导致的速度较低问题待解决
		if (abs((int)(wave_motor_feedback_data->speed_rpm)) < 5)
		{
			*angle_integral = 0;
			wave_motor_speed = 0.0;
			break;
		}
		end_time = osKernelSysTick();
		cal_pid_time += (end_time - start_time);
	}
	*angle_integral = 0;
	wave_motor_speed = 0.0;
	Can2_Send_4Msg(0x1FF, 0, 0, 0, 0);
}

#include "can2_device.h"
#include "gimbal_task.h"
#include "monitor_task.h"
#include "shoot_task.h"
#include "user_commands.h"
#include "cansend_task.h"

extern float easy_pid_p, easy_pid_i, easy_pid_d;
extern float easy_pid2_p, easy_pid2_i, easy_pid2_d;

Pid_Position_t motor_yaw_angle_pid = NEW_POSITION_PID(1.8, 0.2, 0, 15.56, 60, 0, 3000, 500);			  ///< yaw电机角度PID
static Pid_Position_t motor_yaw_speed_pid = NEW_POSITION_PID(800, 0.5, 0.001, 10, 30000, 0, 1000, 500); ///< yaw电机速度PID

static Pid_Position_t motor_pitch_angle_pid = NEW_POSITION_PID(0.35, 0.01, 0.00, 100, 1278, 0, 3000, 500); ///< pitch电机角度PID

static Pid_Position_t friction_motor_left_speed_pid = NEW_POSITION_PID(13, 0, 0.7, 2000, 16383, 0, 1000, 500);
static Pid_Position_t friction_motor_right_speed_pid = NEW_POSITION_PID(13, 0, 0.7, 2000, 16383, 0, 1000, 500);

static Pid_Position_t wave_motor_speed_pid = NEW_POSITION_PID(9, 0, 3, 2000, 16000, 0, 1000, 500);
static Pid_Position_t wave_motor_angle_pid = NEW_POSITION_PID(0.25, 0.018, 0.005, 100, 4500, 0, 3000, 500); ///<  拨轮电机角度PID
/* 自稳云台pitch角度PID参数 */
static Pid_Position_t motor_pitch_angle_pid_imu = NEW_POSITION_PID(0.2, 0.0, 0.0, 100, 1000, 0, 3000, 500);

static int error_integral = 0;
static uint16_t last_machine_angle = 0;
static uint16_t this_machine_angle = 0;
static uint8_t Module_Wave_Wheel = 3;

static uint8_t can2_rxd_data_buffer[8];	   ///< 辅助变量，接受电机反馈的原始数据
static CAN_RxHeaderTypeDef can2_rx_header; ///< 辅助变量，用于 HAL 库函数接受数据，存放关于 CAN 反馈数据的 ID 号等信息
/* 电机数据结构体，解析使用 */
static Motor_Measure_t wave_motor_feedback_data;  ///< 拨轮电机反馈数据
static Motor_Measure_t yaw_motor_feedback_data;	  ///< yaw轴电机反馈数据
static Motor_Measure_t pitch_motor_feedback_data; ///< pitch轴反馈数据

/**
 * @brief 			解析拨轮电机数据
 * @param[in]		void
 * @retval			void
 */
static void Parse_Wave_Motor_Feedback_Data(void)
{
	static uint8_t flag = 1;
	float last_, this_;
	Calculate_Motor_Data(&wave_motor_feedback_data, can2_rxd_data_buffer);
	if (flag)
	{
		flag = 0;
		last_machine_angle = wave_motor_feedback_data.mechanical_angle;
	}
	this_machine_angle = wave_motor_feedback_data.mechanical_angle;
	this_ = this_machine_angle;
	last_ = last_machine_angle;

	Handle_Angle8191_PID_Over_Zero(&this_, &last_);
	error_integral += (this_ - last_);
	last_machine_angle = this_machine_angle;
}
/**
 * @brief	返回误差积分地址
  */
int *Get_Error_Integral(void)
{
	return &error_integral;
}
/**
 * @brief	返回上次机械角度
  */
uint16_t *Get_Last_Machine_Angle(void)
{
	return &last_machine_angle;
}
/**
 * @brief	返回此次机械角度
  */
uint16_t *Get_This_Machine_Angle(void)
{
	return &this_machine_angle;
}
/**
 * @brief	返回云台Yaw轴解析数据
  */
/**
 * @brief                       获取解析后的云台电机数据
 * @param[in]                   void
 * @return {Motor_Measure_t*}   解析后的云台电机数据（数组首地址）
 */
Motor_Measure_t Get_Yaw_Data(void)
{
	return yaw_motor_feedback_data;
}
/**
 * @brief                       获取解析后的云台电机数据
 * @param[in]                   void
 * @return {Motor_Measure_t*}   解析后的云台电机数据（数组首地址）
 */
Motor_Measure_t Get_Pitch_Data(void)
{
	return pitch_motor_feedback_data;
}
/**
 * @brief 			can2 接收回调函数,解析各电机数据
 * @param[in]		void
 * @retval			void
 */
void Can2_Rx_FIFO0_IT_Callback(void)
{
	HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &can2_rx_header, can2_rxd_data_buffer);
	switch (can2_rx_header.StdId)
	{
	case CAN_3508_WAVE_ID:
	{
		Parse_Wave_Motor_Feedback_Data();
		Module_Reload(Module_Wave_Wheel);
	}
	case CAN_YAW_MOTOR_ID:
	{
		Calculate_Motor_Data(&yaw_motor_feedback_data, can2_rxd_data_buffer);
	}
	//掉到CAN1
	// case CAN_PITCH_MOTOR_ID:
	// {
	// 	Can2_Parse_For_Callback();
	// }
	case CAN_PITCH_OutPut_MOTOR_ID:
	{
		Can2_Parse_For_Callback();
	}
	case CAN_3508_FRIC_ID_LEFT:
	case CAN_3508_FRIC_ID_RIGHT:
	{
		Parse_Friction__Data_ISR();
	}
	}
	uint8_t i = 0;
	/* 刷新云台离线检测离线检测 */
	i = can2_rx_header.StdId - CAN_YAW_MOTOR_ID;
	Gimbal_Reload(i);
}

/**
 * @brief                           返回 CAN_RxHeaderTypeDef 轴云台电机在存储电机数组中的下标
 * @param[in]                       void
 * @return CAN_RxHeaderTypeDef*     指向下标的索引
 */
CAN_RxHeaderTypeDef *Get_CAN2_Rx_Header(void)
{
	return &can2_rx_header;
}

/**
 * @brief 				返回 can2 接收到的数组首地址，供各个任务解析
 * @param[in]			void
 * @return uint8_t* 	指向接收数据的数组首地址指针
 */
uint8_t *Get_CAN2_Rxd_Buffer(void)
{
	return can2_rxd_data_buffer;
}

/**
 * @brief 		返回中断解析后的拨轮电机结构体指针
 * @param[in]	void
 * @return 		Motor_Measure_t* 解析后的拨轮电机结构体指针
 */
Motor_Measure_t *Get_Wave_Motor_Paresed_Data(void)
{
	return &wave_motor_feedback_data;
}

/**
 * @brief 									云台串级PID的速度环计算以及向CAN2发送解析后的云台电机速度值
 * @param yaw_speed 						设置的 yaw 轴电机的速度值
 * @param pitch_speed 						设置的 pitch 轴电机的速度值
 * @param yaw_motor_parsed_feedback_data 	指向 yaw 轴电机解析后的结构体指针
 * @param pitch_motor_parsed_feedback_data  指向 pitch 轴电机解析后的结构体指针
 */
void Set_Gimbal_Motors_Speed(float yaw_speed, float pitch_speed, float yaw_speed_rpm, float pitch_speed_rpm)
{
	// motor_pitch_speed_pid.kp = easy_pid_p;
	// motor_pitch_speed_pid.ki = easy_pid_i;
	// motor_pitch_speed_pid.kd = easy_pid_d;
	Can_Send(2,
		CAN_GIMBAL_ALL_ID,
		Pid_Position_Calc(&motor_yaw_speed_pid, yaw_speed, yaw_speed_rpm),
		0,
		0,
		0);
}

/**
 * @brief 				限制 pitch 轴电机的角度
 * @param angle 		当前角度目标值
 * @param down_angle 	最低角度
 * @param up_angle 		最高角度
 */
void Pitch_Angle_Limit(float *angle, float down_angle, float up_angle)
{
	if (down_angle > up_angle)
	{
		if (*angle > down_angle)
			*angle = down_angle;
		else if (*angle < up_angle)
			*angle = up_angle;
	}
	else if (down_angle <= up_angle)
	{
		if (*angle < down_angle)
			*angle = down_angle;
		else if (*angle > up_angle)
			*angle = up_angle;
	}
}

/**
 * @brief 									计算云台Yaw电机的角度环输出
 * @param tar_angle 						设定角度值(倍率后的通道值)
 * @param tar_angle							实际角度值（脱落反馈角度值，空间坐标系下的绝对角度）
 * @param pitch_motor_parsed_feedback_data  指向 yaw 轴电机反馈结构体指针（实际值）
 * @return float 							串级pid角度环输出
 */
float Calc_Yaw_Angle360_Pid(float tar_angle, float cur_angle)
{
	float yaw_tar_angle = tar_angle; ///< ch0 通道值
	float yaw_cur_angle = cur_angle; ///<陀螺仪反馈值

	Handle_Angle360_PID_Over_Zero(&yaw_tar_angle, &yaw_cur_angle);
	return Pid_Position_Calc(&motor_yaw_angle_pid, yaw_tar_angle, yaw_cur_angle);
}

/**
 * @brief 									计算云台电机的角度 pid
 * @param tar_angle 						设定角度值
 * @param pitch_motor_parsed_feedback_data  指向 pitch 轴电机反馈结构体指针
 * @return float 							pid 计算结果
 */
float Calc_Pitch_Angle8191_Pid(float tar_angle, Motor_Measure_t *pitch_motor_parsed_feedback_data)
{
	float pitch_tar_angle = tar_angle;
	float pitch_cur_angle = pitch_motor_parsed_feedback_data->mechanical_angle;

	// motor_pitch_angle_pid.kp = easy_pid2_p;
	// motor_pitch_angle_pid.ki = easy_pid2_i;
	// motor_pitch_angle_pid.kd = easy_pid2_d;

	Handle_Angle8191_PID_Over_Zero(&pitch_tar_angle, &pitch_cur_angle);
	return Pid_Position_Calc(&motor_pitch_angle_pid, pitch_tar_angle, pitch_cur_angle); ///< 这是第一层 PID，计算设定角度与实际角度之间的误差，得到下一步要设定的速度值，如果已经达到目标值，则输出为 0
}

float Calc_Wave_Motor_Angle8191_Pid(float tar_angle, float current_angle)
{
	float wave_motor_tar_angle = tar_angle;
	float wave_motor_cur_angle = current_angle;
	 Handle_Angle8191_PID_Over_Zero(&tar_angle, &current_angle);
	return Pid_Position_Calc(&wave_motor_angle_pid, wave_motor_tar_angle, wave_motor_cur_angle); ///< 这是第一层 PID，计算设定角度与实际角度之间的误差，得到下一步要设定的速度值，如果已经达到目标值，则输出为 0
}

/**
 * @brief 								设置拨轮电机的速度(包含速度环结算)
 * @param speed_left 					左边拨轮电机的速度
 * @param speed_right 					右边拨轮电机的速度
 * @param wave_motor_feedback_data 		两个拨轮电机解析后的反馈数据
 */
void Set_Friction_Motor_Speed(float speed_left, float speed_right, Motor_Measure_t *friction_motor_feedback_data)
{
	int16_t left = Pid_Position_Calc(&friction_motor_left_speed_pid, speed_left, friction_motor_feedback_data[0].speed_rpm);
	int16_t right = Pid_Position_Calc(&friction_motor_right_speed_pid, speed_right, friction_motor_feedback_data[1].speed_rpm);
	Can_Send(2,
		CAN_SHOOTER_ALL_ID,
		left,
		right,
		0,
		0);
}

void Set_Wave_Motor_Speed(float wave_motor_speed, Motor_Measure_t *wave_motor_feedback_data)
{
	int16_t wave_motor_pid_out_speed = Pid_Position_Calc(&wave_motor_speed_pid, wave_motor_speed, wave_motor_feedback_data->speed_rpm);
	Can_Send(2,
		0x1FF,
		0,
		wave_motor_pid_out_speed,
		0,
		0);
}

/**
 * @brief 								计算自稳云台模式的角度环PID
 * @param tar_angle 					目标角度
 * @param imu_on_broad 					反馈角度（陀螺仪pit）
 * @retval 								角度环输出
 */
float Calc_Pitch_Angle8191_Imu_Pid(float tar_angle, float imu_angle_data)
{
	float pitch_tar_angle = tar_angle;
	float pitch_cur_angle = imu_angle_data;
	Handle_Angle8191_PID_Over_Zero(&pitch_tar_angle, &pitch_cur_angle);
	return Pid_Position_Calc(&motor_pitch_angle_pid_imu, pitch_tar_angle, pitch_cur_angle);
	///< 这是第一层 PID，计算设定角度与实际角度之间的误差，得到下一步要设定的速度值，如果已经达到目标值，则输出为 0
}

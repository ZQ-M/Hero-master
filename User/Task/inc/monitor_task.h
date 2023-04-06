#ifndef __MONITOR_TASK_H_
#define __MONITOR_TASK_H_

#include "cmsis_os.h"
#include "monitor.h"
#include "led.h"
#include "usart3.h"
#include "buzzer.h"
#include "can1_device.h"

enum moduleList
{
	remote_control = 0u, //遥控器
	chassis_motor,       //底盘电机
	gimbal_motor,        //云台电机
	shooter_motor,       //发射机构电机
	auto_aim,            //自瞄NUC
	judge_system,        //裁判系统
	super_capacitor,     //超级电容
	gyroscope            //陀螺仪
};

uint8_t Get_Module_Online_State(uint8_t id);
void Module_Reload(uint8_t index);
void Classis_Reload(int8_t motor_index);
void Gimbal_Reload(int8_t motor_index);

#endif // __MONITOR_TASK_H_

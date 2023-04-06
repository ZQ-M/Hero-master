#include "monitor_task.h"
#include "referee_system.h"
#include "super_capacitor_task.h"



static Module_status_t module_status[8]; ///< 模块数量

static const Judge_data_t *referee_date_pt;
static const Super_Capacitor_t *super_capacitor_date;
static void Monitor_Task_Init(void);   ///< 初始化监听任务
static void Monitor_All_Modules(void); ///< 执行监听

void StartMonitorTask(void const *argument)
{
	Monitor_Task_Init(); ///< 监听任务初始化
	Led_Flow_Flash();	 ///< 初始化流水灯
	Buzzer_Bsp_Init();	 ///< 初始化时蜂鸣
	uint8_t cap_send_cnt = 5;
	referee_date_pt = Get_Referee_Data();
	super_capacitor_date = Get_SuperCapacitor_Date();



	osDelay(50);

	for (;;)
	{
		Monitor_All_Modules();

		if (cap_send_cnt > 2)
		{
			/* 功率限制 */
			if (module_status[judge_system].time_out_flag == 0 && module_status[super_capacitor].time_out_flag == 0) //判断裁判系统、底盘是否同时上线
			{
				//判断超级电容目标功率与裁判系统限制功率-2是否相符，否设置超级电容
				if(referee_date_pt->power_heat_data.chassis_power - 2!= (uint8_t)super_capacitor_date->target_power);
				{
					Set_Super_Capacitor((referee_date_pt->power_heat_data.chassis_power - 2) * 100);
					cap_send_cnt = 0;
				}
			}
		}
		else
		{
			cap_send_cnt++;
		}
		osDelay(200);
	}
}

/**
 * @brief 		获取指定模块的在线状态
 * @param 		{uint8_t} id moduleList的枚举成员
 * @return		0 & 1 在线状态
 */
uint8_t Get_Module_Online_State(uint8_t id)
{
	return Get_Module_Status(&module_status[id]);
}

/**
 * @brief 	模块状态重置
 * @param 	{uint8_t} id moduleList的枚举成员
 * @return	 {*}
 */
void Module_Reload(uint8_t index)
{
	Module_Status_Reload(&module_status[index]);
}
/**
 * @brief 	底盘重装载
 * @param 	{int8_t} 电机ID
 * @return	 {*}
 */
void Classis_Reload(int8_t motor_index)
{
	static uint8_t chassis_motor[4] = {0, 0, 0, 0};

	if (motor_index >= 0 && motor_index <= 3)
	{
		chassis_motor[motor_index] = 1;
	}
	else
	{
		return;
	}
	if (chassis_motor[0] && chassis_motor[1] && chassis_motor[2] && chassis_motor[3])
	{
		chassis_motor[0] = 0;
		chassis_motor[1] = 0;
		chassis_motor[2] = 0;
		chassis_motor[3] = 0;
		Module_Status_Reload(&module_status[1]);
	}
}
/**
 * @brief: 云台重装载
 * @param {int8_t} motor_index must be 1 or 2. 为1时重装载yaw轴电机，2装载pitch电机。
 * @return {*}
 */
void Gimbal_Reload(int8_t motor_index)
{
	// bit0代表电机yaw bit1代表电机pitch
	static uint8_t gimbal_motor_state = 0;

	if (motor_index >= 1 && motor_index <= 2)
	{
		gimbal_motor_state |= (0x01 << (motor_index - 1));
	}
	else
	{
		return;
	}
	if ((gimbal_motor_state & 0x03) == 0x03)
	{
		gimbal_motor_state = 0;
		Module_Status_Reload(&module_status[gimbal_motor]);
	}
}

/**
 * @brief 初始化监听任务
 * @param {*}
 * @return {*}
 */
static void Monitor_Task_Init(void)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		Module_Status_Init(&module_status[i], 5, NULL, NULL);
	}
}

/**
 * @brief 执行监听
 * @param {*}
 * @return {*}
 */
static void Monitor_All_Modules(void)
{
#define MODULE_REONLINE(index) (module_status[index].time_out_flag == 0 && module_status[index].old_time_out_flag == 1)
#define MODULE_OFFLINE(index) (module_status[index].time_out_flag == 1 && module_status[index].old_time_out_flag == 0)

	for (uint8_t i = 0; i < 8; i++)
	{
		//各个模块状态刷新
		Module_Status_Listen(&module_status[i]);

		if (MODULE_REONLINE(i))
		{
			LED_FLOW_ON(i);
			info_log("Module%d Online.\r\n\r\n", i);
		}
		else if (MODULE_OFFLINE(i))
		{
			LED_FLOW_OFF(i);
			info_log("Module%d Offline.\r\n\r\n", i);
		}
	}
}

#undef MODULE_OFFLINE
#undef MODULE_REONLINE

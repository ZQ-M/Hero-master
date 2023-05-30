#include "motor.h"

void Calculate_Motor_Data(Motor_Measure_t* motor, uint8_t* can_message)
{
	motor->mechanical_angle = (uint16_t)(can_message[0] << 8 | can_message[1]);
	motor->speed_rpm = (uint16_t)(can_message[2] << 8 | can_message[3]);
	motor->actual_torque_current = (uint16_t)(can_message[4] << 8 | can_message[5]);
	motor->temperate = can_message[6];
}

void Calculate_PITCH_Angle(Motor_Measure_t* motor, uint8_t* can_message)
{
	motor->mechanical_angle = (uint16_t)(can_message[4] << 8 | can_message[3]);
	motor->speed_rpm = 0;
	motor->actual_torque_current = 0;
	motor->temperate = 0;
}

/*
 * @param 判断42步进电机是否接收到CAN数据
 * @param can message
 * @return 是否接收到了正确的数据
 * @note 42步进电机固定返回CAN_ID:0x002的三位数据 0x02 0x02 0x6B 
 */
int Calculate_Pitch_Motor_CallBackData(uint8_t* can_message)
{
	if(can_message[0] == 0x02 && can_message[1] == 0x02 && can_message[2] == 0x6B)
	{
		return 1;
	}
	else{return 0;}
}

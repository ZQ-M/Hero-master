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
 * @param �ж�42��������Ƿ���յ�CAN����
 * @param can message
 * @return �Ƿ���յ�����ȷ������
 * @note 42��������̶�����CAN_ID:0x002����λ���� 0x02 0x02 0x6B 
 */
int Calculate_Pitch_Motor_CallBackData(uint8_t* can_message)
{
	if(can_message[0] == 0x02 && can_message[1] == 0x02 && can_message[2] == 0x6B)
	{
		return 1;
	}
	else{return 0;}
}

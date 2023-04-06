#include "motor.h"

void Calculate_Motor_Data(Motor_Measure_t* motor, uint8_t* can_message)
{
	motor->mechanical_angle = (uint16_t)(can_message[0] << 8 | can_message[1]);
	motor->speed_rpm = (uint16_t)(can_message[2] << 8 | can_message[3]);
	motor->actual_torque_current = (uint16_t)(can_message[4] << 8 | can_message[5]);
	motor->temperate = can_message[6];
}


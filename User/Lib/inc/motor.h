#ifndef __MOTOR_H_
#define __MOTOR_H_

#include "stm32f4xx.h"

typedef struct
{
    uint16_t mechanical_angle;     ///< 转子机械角度值范围 0~8191 对应转子机械角度为 0~360
    int16_t speed_rpm;             ///< 转子转速值单位 : RPM
    int16_t actual_torque_current; ///< 实际转矩电流值
    uint8_t temperate;             ///< 电机温度
    int16_t last_mechanical_angle; ///< 电机上次机械角度值
} Motor_Measure_t;

/* read motor data and parse them */
#define GET_MOTOR_MEASURE_DATA(ptr, data)                                      \
    {                                                                          \
        (ptr)->last_mechanical_angle = (ptr)->mechanical_angle;                \
        (ptr)->mechanical_angle = (uint16_t)((data)[0] << 8 | (data)[1]);      \
        (ptr)->speed_rpm = (uint16_t)((data)[2] << 8 | (data)[3]);             \
        (ptr)->actual_torque_current = (uint16_t)((data)[4] << 8 | (data)[5]); \
        (ptr)->mechanical_angle = (data)[6];                                   \
    }

void Calculate_Motor_Data(Motor_Measure_t* motor, uint8_t* can_message);

#endif // __MOTOR_H_
		
		

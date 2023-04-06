#ifndef __CAN1_DEVICE_H_
#define __CAN1_DEVICE_H_

#include "can1.h"
#include "pid.h"
#include "stm32f4xx.h"

typedef enum
{
    CAN_CHASSIS_ALL_ID = 0x200,
    CAN_3508_M1_ID = 0x201,
    CAN_3508_M2_ID = 0x202,
    CAN_3508_M3_ID = 0x203,
    CAN_3508_M4_ID = 0x204,

    SUPER_CAPACITOR_ID = 0x211,
    SUPER_CAPACITOR_SEND_ID = 0x210,
} Can1_RxMsg_ID;
void Can1_Process(CAN_RxHeaderTypeDef *can1_rx_message);///< 中断调用的CAN函数
void Set_ChassisMotor_Speed(float speed_fl, float speed_fr, float speed_bl, float speed_br, Motor_Measure_t *chassis_motor_feedback_data);
void Set_Super_Capacitor(uint16_t target_power);///< 超级电容数据发送函数声明
#endif // __CAN1_DEVICE_H_

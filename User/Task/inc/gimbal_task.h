#ifndef __GIMBAL_TASK_H_
#define __GIMBAL_TASK_H_

#include "can2.h"
#include "can2_device.h"
#include "cmsis_os.h"
#include "config.h"
#include "config.h" ///< 机器人配置数据
#include "console_print.h"
#include "imu_mpu6050.h"
#include "math2.h"
#include "pid.h"
#include "remote_task.h"
/*yaw*/
#define YAW_SPEED_OUTPUT_LIMIT(data, limit) Float_Constraion(data, -limit, limit)

/*pitch*/
/*测量极限位置的电机角度返回值得到这些数据。如云台最高&最低能到达的位置*/

#define PITCH_UP_LIMIT pitch_up_angle_limit     ///<pitch上限位
#define PITCH_MID_ANGLE pitch_middle_angle      ///<pitch中间附近
#define PITCH_DOWN_LIMIT pitch_down_angle_limit ///<pitch下限位

#define __OPEN_CAN2_RX_FIFO0_IT__ HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
/*对外函数声明*/
void Parse_Can2_Gimbal_Rxd_Data(CAN_RxHeaderTypeDef *p_can_rx_header, uint8_t data[], Motor_Measure_t *motor);
Motor_Measure_t *Get_Gimbal_Parsed_FeedBack_Data(void);
const uint8_t *Get_Pitch_Motor_Index(void);
const uint8_t *Get_Yaw_Motor_Index(void);
float GM6020_YAW_Angle_To_360(uint16_t gm6020_angle); ///<电机返回机械角数据转换为度数格式
void Can2_Parse_For_Callback(void);
#endif //__GIMBAL_TASK_H_

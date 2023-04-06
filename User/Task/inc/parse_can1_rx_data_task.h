#ifndef __PARSE_CAN1_RX_DATA_TASK_H_
#define __PARSE_CAN1_RX_DATA_TASK_H_

#include "cmsis_os.h"
#include "stm32f4xx.h"
#include "usart3.h"
#include "motor.h"
#include "can1.h"
#include "can1_device.h"

void Info_Can1_ParseData_Task(void);
void Parse_Can1_Rxd_Data(void);
Motor_Measure_t *Get_Can1_Feedback_Data(void);
const uint8_t *Get_Can1_Motor_DeviceNumber(void);

#endif //__PARSE_CAN1_RX_DATA_TASK_H_

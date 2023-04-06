#ifndef __REFERENCE_ANALYSES_TASK_H_
#define __REFERENCE_ANALYSES_TASK_H_

#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "console_print.h"
#include "monitor_task.h"
#include "referee_system.h"
#include "uart8.h"
#include "usart3.h"
void Inform_Referee_System_Task_With_len(uint32_t send_date); ///< 通知裁判系统解析，在Uart8中断中调用
void Inform_Reference_Analysis_Task_Pasre_Data(void);
#endif // __REFERENCE_ANALYSES_TASK_H_

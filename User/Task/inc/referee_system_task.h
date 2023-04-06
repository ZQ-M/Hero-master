#ifndef _REFEREE_SYSTEM_H_
#define _REFEREE_SYSTEM_H_

#include "usart6.h"
#include "stm32f4xx.h"
#include "console_print.h"
#include "cmsis_os.h"
#include "monitor_task.h"

void Info_Referee_System_Task_Parse_Data(void);
void Inform_Referee_System_Task_With_len(uint32_t send_date);///< 直接任务通知函数声明
#endif //_REFEREE_SYSTEM_H_

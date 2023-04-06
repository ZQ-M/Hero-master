#ifndef __BUZZER_TASK_H_
#define __BUZZER_TASK_H_

#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "buzzer.h"
#include "usart3.h"

typedef struct
{
	uint8_t beep_time;    ///< 滴滴数量
	uint16_t pulse;         ///< 声音频率
	uint16_t delay_time;  ///< 滴滴及滴滴间隔时间
}Beep_Play_t;

osStatus Set_Beep_Time(uint8_t beep_time, uint16_t pulse, uint16_t delay_time);

#endif //__BUZZER_TASK_H_

#ifndef __MONITOR_H_
#define __MONITOR_H_

#include "stm32f4xx.h"
#include "stdio.h"

typedef struct
{
	uint16_t count;
	uint16_t reload_cnt;

	uint8_t time_out_flag;
	uint8_t old_time_out_flag;
	
	void (*Module_Online_Callback)(void);
	void (*Module_Offline_Callback)(void);
}Module_status_t;

void Module_Status_Init(Module_status_t* module, uint8_t cnt_num, void Online_Callback(void), void Offline_Callback(void));
void Module_Status_Listen(Module_status_t* module);
uint8_t Get_Module_Status(Module_status_t* module);
void Module_Status_Reload(Module_status_t* module);

#endif // __MONITOR_H_

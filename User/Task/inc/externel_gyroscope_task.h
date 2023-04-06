#ifndef __EXTERNEL_GYROSCOPE_TASK_H_
#define __EXTERNEL_GYROSCOPE_TASK_H_

#include "cmsis_os.h"
#include "console_print.h"
#include "monitor_task.h"
#include "usart6.h"

typedef struct
{
	struct
	{
		float x;
		float y;
		float z;
	} acceleration; //加速度

	struct
	{
		float roll_x;
		float pitch_y;
		float yaw_z;
	} angle; //角度

	float temperature;

} Wt61c_Data_t;

void Inform_Gyroscope_Task_Parse_Data(void);
uint8_t Check_Wt61c_Data_Available(const uint8_t *wt61c_raw_buf, uint16_t length);
Wt61c_Data_t *Get_Gyroscope_Data_t(void);

#endif // __EXTERNEL_GYROSCOPE_TASK_H_

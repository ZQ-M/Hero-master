#include "default_task.h"

void StartDefaultTask(void const *argument)
{
	/* 关闭红绿LED */
	LED_GREEN_OFF();
	LED_RED_OFF();

	Laser_ON(); ///<打开激光
	for (;;)
	{
		LED_GREEN_TOGGLE();//绿色LED切换，用来判断系统是否卡死，正常情况是0.5s反转一次
		osDelay(500);
	}
}

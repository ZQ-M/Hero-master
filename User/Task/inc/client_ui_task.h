#ifndef _CLIENT_UI_TASK_H_
#define _CLIENT_UI_TASK_H_

#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "uart8.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "client_ui.h"
#include "judge_system.h"
#include "remote.h"
#include "super_capacitor_task.h"

void OperatorUItask(void const *argument);
void GrenadeCueLine(uint8_t Color);
void FrictionWheelSpeed(void);
void Last_42mm_Speed(void);
void Super_Capacitance(void);
void KeyG_Clicked_Status(void);

#endif // _CLIENT_UI_TASK_H_


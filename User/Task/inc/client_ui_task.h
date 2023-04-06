#ifndef _CLIENT_UI_TASK_H_
#define _CLIENT_UI_TASK_H_

#include "cmsis_os.h"
#include "uart8.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "client_ui.h"
#include "judgment_system.h"

void OperatorUItask(void const *argument);
void GrenadeCueLine(uint8_t Color);
void FrictionWheelSpeed(void);
void Last_42mm_Speed(void);
void Super_Capacitance(void);

#endif // _CLIENT_UI_TASK_H_


#ifndef __CANSEND_TASK
#define __CANSEND_TASK

#include "stm32f4xx.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

extern TaskHandle_t CansendTask_Handler;

void Cansend_Task(void *pvParameters);

BaseType_t Can_Send(uint8_t canx, uint32_t id, int16_t data1, int16_t data2, int16_t data3, int16_t data4);

#endif
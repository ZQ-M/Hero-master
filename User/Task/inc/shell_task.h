#ifndef __SHELL_TASK_
#define __SHELL_TASK_

#include "cmsis_os.h"
#include "stm32f4xx.h"
#include "usart.h"
#include "stdio.h"
#include "usart3.h"
#include "stm32f4xx_ll_usart.h."
#include "usart3.h"
#include "shell.h"

#define PRINT_USART_INIT() {Usart3_Tx_Init();Usart3_Rx_Init();}

void Usart3_Idle_ITCallback(void);
void Print_Dragon(void);
void Print_Logo(void);

#endif // __SHELL_TASK_

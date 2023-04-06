#ifndef __USART1_H_
#define __USART1_H_

#include "stm32f4xx_ll_usart.h"
#include "usart.h"
#include "remote_task.h"

void Usart1_DMA_RxCp_Callback(void);
void Usart1_RxDMA_Init(void);
uint8_t *Get_Usart1_DMA_RxBuffer_One(void);
uint8_t *Get_Usart1_DMA_RxBuffer_Two(void);
const uint16_t *Get_Usart1_DMA_RxMaxLen(void);
uint16_t *Get_Usart1_DMA_Rxd_DataLen(void);
uint8_t Get_Rc_Available_Bufferx(void);
void Usart1_Rx_DMA_Reset(void);

#endif //__USART1_H_

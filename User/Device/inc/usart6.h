#ifndef __USART6_H_
#define __USART6_H_

#include "stm32f4xx_ll_usart.h"
#include "usart.h"
#include "referee_system_task.h"

void Usart6_DMA_RxCp_Callback(void);
void Usart6_RxDMA_Init(void);
uint8_t *Get_Usart6_DMA_RxBuffer_One(void);
uint8_t *Get_Usart6_DMA_RxBuffer_Two(void);
const uint16_t *Get_Usart6_DMA_RxMaxLen(void);
uint16_t *Get_Usart6_DMA_Rxd_DataLen(void);
uint8_t Get_USART6_DMA_Available_Bufferx(void);
void Usart6_Rx_DMA_Reset(void);
uint8_t Get_Usart6_Available_Bufferx(void);

#endif //__USART1_H_

#ifndef __UART8_H_
#define __UART8_H_

#include "stm32f4xx.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_dma.h"
#include "reference_analysis_task.h"

void Uart8_Rx_Dma_Init(void);
void UART8_DMA_RxCp_Callback(void);
uint8_t Get_Reference_Available_Bufferx(void);
void Uart8_Rx_DMA_Reset(void);
uint16_t *Get_Uart8_DMA_Rxd_DataLen(void);
uint8_t *Get_Uart8_DMA_RxBuffer_Two(void);
uint8_t *Get_Uart8_DMA_RxBuffer_One(void);
int printf_ui(const char *format, ...);
void Uart8_Dma_Tx_Init(void);
void Uart8_Transmit_Dma(uint32_t data_address, uint32_t len);
void Uart8_It_Tc_Callback(void);

#endif // __UART8_H_

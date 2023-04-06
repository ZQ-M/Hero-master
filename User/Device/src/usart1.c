#include "usart1.h"

static const uint16_t usart1_dma_rx_max_len = 36;					  ///< USART1 DMA 最大接收长度
static volatile uint8_t usart1_dma_rx_buffer0[usart1_dma_rx_max_len]; ///< USART1 DMA 接受缓冲区 1
static volatile uint8_t usart1_dma_rx_buffer1[usart1_dma_rx_max_len]; ///< USART1 DMA 接受缓冲区 2
static volatile uint16_t usart1_dma_rxd_data_len;					  ///< USART1 DMA 已经接收到的数据长度

/**
 * @brief       初始化串口 1 的接收 DMA
 * @param[in]   none
 * @retval      void
 */
void Usart1_RxDMA_Init(void)
{
	LL_DMA_SetPeriphAddress(DMA2, LL_DMA_STREAM_2, LL_USART_DMA_GetRegAddr(USART1));
	LL_DMA_SetMemoryAddress(DMA2, LL_DMA_STREAM_2, (uint32_t)(usart1_dma_rx_buffer0));
	LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, usart1_dma_rx_max_len);

	///< 开启 DMA 双缓冲
	LL_DMA_SetMemory1Address(DMA2, LL_DMA_STREAM_2, (uint32_t)(usart1_dma_rx_buffer1));
	LL_DMA_SetCurrentTargetMem(DMA2, LL_DMA_STREAM_2, LL_DMA_CURRENTTARGETMEM0);
	LL_DMA_EnableDoubleBufferMode(DMA2, LL_DMA_STREAM_2);

	LL_USART_ClearFlag_IDLE(USART1);
	LL_USART_EnableIT_IDLE(USART1);

	LL_USART_EnableDMAReq_RX(USART1);

	LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);
}

/**
 * @brief           串口 1 的接收中断函数
 * @param[in]       none
 * @retval          void
 */
void Usart1_DMA_RxCp_Callback(void)
{
	if (LL_USART_IsActiveFlag_IDLE(USART1))
	{
		///< 关闭 DMA
		LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_2);

		///< 清楚 IDLE 中断、DMA TC2 标志位
		LL_USART_ClearFlag_IDLE(USART1);
		LL_DMA_ClearFlag_TC2(DMA2);

		///< 获取该帧的数据长度
		usart1_dma_rxd_data_len = usart1_dma_rx_max_len - LL_DMA_GetDataLength(DMA2, LL_DMA_STREAM_2);

		if (LL_DMA_GetCurrentTargetMem(DMA2, LL_DMA_STREAM_2) == LL_DMA_CURRENTTARGETMEM1)
		{
			LL_DMA_SetCurrentTargetMem(DMA2, LL_DMA_STREAM_2, LL_DMA_CURRENTTARGETMEM0);
		}
		else
		{
			LL_DMA_SetCurrentTargetMem(DMA2, LL_DMA_STREAM_2, LL_DMA_CURRENTTARGETMEM1);
		}

		///< 重新设置数据传输长度
		LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, usart1_dma_rx_max_len);

		///< 通知任务进行解析
		Info_RemoteTask_Parse_Data();

		///< 重新开启 DMA
		LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);
	}
}

/**
 * @brief           返回 USART1 DMA 的第一个接受缓冲区指针
 * @param[in]       none
 * @retval          USART1 DMA 的接受缓冲区地址
 */
uint8_t *Get_Usart1_DMA_RxBuffer_One()
{
	return (uint8_t *)usart1_dma_rx_buffer0;
}

/**
 * @brief           返回 USART1 DMA 的接受缓冲区最大接受长度
 * @param[in]       none
 * @retval          USART1 DMA 的最大接收长度
 */
const uint16_t *Get_Usart1_DMA_RxMaxLen(void)
{
	return &usart1_dma_rx_max_len;
}

/**
 * @brief           返回 USART1 DMA 的接受缓冲区已经接受到的数据长度
 * @param[in]       none
 * @retval          USART1 DMA 已经接受到了多长的数据
 */
uint16_t *Get_Usart1_DMA_Rxd_DataLen(void)
{
	return (uint16_t *)&usart1_dma_rxd_data_len;
}

/**
 * @brief           返回 USART1 DMA 第二个接收缓冲区
 * @param[in]       none
 * @retval          USART1 DMA 的接受缓冲区地址
 */
uint8_t *Get_Usart1_DMA_RxBuffer_Two(void)
{
	return (uint8_t *)(usart1_dma_rx_buffer1);
}

/**
 * @brief           返回 USART1 DMA 的当前可用的接受缓冲区
 * @param[in]       none
 * @retval          USART1 DMA 的当前可用的接受缓冲区
 */
uint8_t Get_Rc_Available_Bufferx(void)
{
	if (LL_DMA_GetCurrentTargetMem(DMA2, LL_DMA_STREAM_2))
	{
		return 1;
	}
	return 0;
}

/**
 * @brief           重置 USART1 Rx DMA
 * @param[in]       none
 * @retval          void
 */
void Usart1_Rx_DMA_Reset(void)
{
	LL_USART_Disable(USART1);
	LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_2);
	LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, usart1_dma_rx_max_len);

	LL_USART_ClearFlag_IDLE(USART1);
	LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);
	LL_USART_Enable(USART1);
}

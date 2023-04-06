#include "usart3.h"

extern osMutexId printfMutexHandle;

static volatile uint8_t usart3_tx_lock = 0;
static volatile char usart3_dma_transmit_buf[256];
static const uint16_t usart3_dma_rx_max_len = 50;
volatile uint8_t usart3_dma_rx_buffer[usart3_dma_rx_max_len];

/**
 * @brief 串口3发送额外初始化
 * @param {*}
 * @return {*}
 */
void Usart3_Tx_Init(void)
{
	LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_3, LL_USART_DMA_GetRegAddr(USART3));
	LL_USART_EnableDMAReq_TX(USART3);
}

/**
 * @brief 串口3使用DMA方式发送
 * @param {uint32_t} data_address 发送的数据地址
 * @param {uint32_t} len 发送的数据长度
 * @return {*}
 */
void Usart3_Transmit_Dma(uint32_t data_address, uint32_t len)
{
	LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_3, data_address);
	LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_3, len);
	LL_USART_ClearFlag_TC(USART3);
	LL_DMA_ClearFlag_TC3(DMA1);
	// LL_USART_EnableIT_TC(USART3);
	LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_3);
}

/**
 * @brief 串口3 TC中断回调函数（需在中断中调用）
 * @param {*}
 * @return {*}
 */
void Usart3_It_Tc_Callback(void)
{
	// usart3_tx_lock = 0;
	LL_USART_DisableIT_TC(USART3);
	LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_3);
}

void Usart3_Rx_Init(void)
{
	LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_1, LL_USART_DMA_GetRegAddr(USART3));
	LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_1, (uint32_t)(usart3_dma_rx_buffer));

	LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_1, usart3_dma_rx_max_len);

	LL_USART_ClearFlag_IDLE(USART3);
	LL_USART_EnableIT_IDLE(USART3);

	LL_USART_EnableDMAReq_RX(USART3);

	LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_1);
}

uint8_t *Get_Usart3_DMA_RxBuffer(void)
{
	return (uint8_t *)usart3_dma_rx_buffer;
}

const uint16_t *Get_Usart3_DMA_Rx_MaxLen(void)
{
	return &usart3_dma_rx_max_len;
}

/**
 * @brief printf函数（不建议直接调用）
 * @note 会占用红灯，当等待发送时红灯亮
 * @param {char} *format 同printf
 * @return {int} 打印的字符长度
 */
static uint8_t first_release_mutex_flag = 0;

int __printf(const char *format, ...)
{
	
	if (first_release_mutex_flag == 0)
	{
		first_release_mutex_flag = 1;
		osMutexRelease(printfMutexHandle);
	}
	osMutexWait(printfMutexHandle, osWaitForever);

	uint32_t len;
	va_list args;
	va_start(args, format);
	len = vsnprintf((char *)usart3_dma_transmit_buf, sizeof(usart3_dma_transmit_buf), (const char *)format, args);
	va_end(args);
	Usart3_Transmit_Dma((uint32_t)usart3_dma_transmit_buf, len);
	LED_RED_OFF();
	while (!((USART3->SR) & USART_SR_TC))
	{
		LED_RED_ON();
	}
	///< 关闭 DMA
	LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_3);
	LED_RED_OFF();
	///< 这里释放锁
	osMutexRelease(printfMutexHandle);

	return -1;
}

static char *error = ">>ERROR>>";
int __printf_error(const char *format, ...)
{
	
	if (first_release_mutex_flag == 0)
	{
		first_release_mutex_flag = 1;
		osMutexRelease(printfMutexHandle);
	}
	osMutexWait(printfMutexHandle, osWaitForever);

	uint32_t len;
	va_list args;
	va_start(args, format);
	len = vsnprintf((char *)usart3_dma_transmit_buf, sizeof(usart3_dma_transmit_buf), (const char *)format, args);
	va_end(args);
	strcat((char *)usart3_dma_transmit_buf, error);
	Usart3_Transmit_Dma((uint32_t)usart3_dma_transmit_buf, len + strlen(error));
	LED_RED_OFF();
	while (!((USART3->SR) & USART_SR_TC))
	{
		LED_RED_ON();
	}
	///< 关闭 DMA
	LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_3);
	LED_RED_OFF();
	///< 这里释放锁
	osMutexRelease(printfMutexHandle);

	return -1;
}

static char *log = ">>LOG>> ";
int __printf_log(const char *format, ...)
{
	
	if (first_release_mutex_flag == 0)
	{
		first_release_mutex_flag = 1;
		osMutexRelease(printfMutexHandle);
	}
	osMutexWait(printfMutexHandle, osWaitForever);

	uint32_t len;
	va_list args;
	va_start(args, format);
	len = vsnprintf((char *)usart3_dma_transmit_buf, sizeof(usart3_dma_transmit_buf), (const char *)format, args);
	va_end(args);
	strcat((char *)usart3_dma_transmit_buf, log);
	Usart3_Transmit_Dma((uint32_t)usart3_dma_transmit_buf, len + strlen(log));
	LED_RED_OFF();
	while (!((USART3->SR) & USART_SR_TC))
	{
		LED_RED_ON();
	}
	///< 关闭 DMA
	LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_3);
	LED_RED_OFF();
	///< 这里释放锁
	osMutexRelease(printfMutexHandle);

	return -1;
}

static char *warning = ">>WARNING>> ";
int __printf_warning(const char *format, ...)
{
	
	if (first_release_mutex_flag == 0)
	{
		first_release_mutex_flag = 1;
		osMutexRelease(printfMutexHandle);
	}
	osMutexWait(printfMutexHandle, osWaitForever);

	uint32_t len;
	va_list args;
	va_start(args, format);
	len = vsnprintf((char *)usart3_dma_transmit_buf, sizeof(usart3_dma_transmit_buf), (const char *)format, args);
	va_end(args);
	strcat((char *)usart3_dma_transmit_buf, warning);
	Usart3_Transmit_Dma((uint32_t)usart3_dma_transmit_buf, len + strlen(warning));
	LED_RED_OFF();
	while (!((USART3->SR) & USART_SR_TC))
	{
		LED_RED_ON();
	}
	///< 关闭 DMA
	LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_3);
	LED_RED_OFF();
	///< 这里释放锁
	osMutexRelease(printfMutexHandle);

	return -1;
}

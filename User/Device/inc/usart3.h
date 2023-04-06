#ifndef __USART3_H_
#define __USART3_H_

#include "stm32f4xx.h"
#include "stdarg.h"
#include "stdio.h"
#include "usart.h"
#include "led.h"
#include "stdlib.h"
#include "string.h"
#include "config.h"

#if INFORMATION
	#define info_print(format, ...) __printf(format, ##__VA_ARGS__)
	#define info_log(format, arg...) __printf("<INFO> " format "\r\n", ##arg)
#else
	#define info_print(format, ...)
	#define info_log(format, arg...)
#endif

#if DEBUG //定义在Config.h
	#define debug_print(format, ...) __printf(format, ##__VA_ARGS__)
	#define debug_log(format, arg...) __printf("<DEBUG> " format "\r\n", ##arg)
	#define debug_showdata1(name, data) __printf("<DEBUG> %s = %d\r\n",name,data)
	#define debug_showdata2(name, data) __printf("<DEBUG> %s = %.2f\r\n",name,data)
	#define debug_error(err) __printf("<ERROR> error:%d\r\n",err)
	#define debug_array1(name, data, len) { __printf("<DEBUG>%s : {",name); for(uint16_t __tmp_i=0; __tmp_i<len; __tmp_i++) __printf("%d ", data[__tmp_i]);  __printf("}\r\n"); }
    #define debug_array2(name, data, len) { __printf("<DEBUG>%s : {",name); for(u16 __tmp_i=0; __tmp_i<len; __tmp_i++) __printf("%.2f ", data[__tmp_i]);  __printf("}\r\n"); }
#else
	#define debug_print(format, ...)
	#define debug_log(format, arg...)
	#define debug_showdata1(name, data)
	#define debug_showdata2(name, data)
	#define debug_error(err)
	#define debug_array1(name, data, len)
	#define debug_array2(name, data, len)
#endif

void Usart3_Tx_Init(void);
void Usart3_Transmit_Dma(uint32_t data_address, uint32_t len);
void Usart3_It_Tc_Callback(void);
int __printf(const char *format, ...);
int __printf_error(const char *format, ...);
int __printf_log(const char *format, ...);
int __printf_warning(const char *format, ...);

void Usart3_Rx_Init(void);
uint8_t *Get_Usart3_DMA_RxBuffer(void);
const uint16_t *Get_Usart3_DMA_Rx_MaxLen(void);
#endif //__USART3_H_

#ifndef __CONSOLE_PRINT_H_
#define __CONSOLE_PRINT_H_

#include "usart3.h"

/**
 * @brief 串口打印函数指针结构体
 */
typedef struct
{
    int (*print)(const char *format, ...);
    int (*log)(const char *format, ...);
    int (*warning)(const char *, ...);
    int (*error)(const char *, ...);

} Console_t;

extern const Console_t Console;

#endif //__CONSOLE_PRINT_H_

#include "console_print.h"

/**
 * @brief 建议直接使用 Console.print() 类似于普通的 printf 函数
 */
const Console_t Console = {
    .print = __printf,
    .log = __printf_log,
    .error = __printf_error,
    .warning = __printf_warning
};

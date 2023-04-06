#ifndef __SHELLIO_H
#define __SHELLIO_H

#ifndef NULL
    // #define NULL ((void *)0)
    #include "stdio.h"
#endif

#define STR_COMPARE(str1, str2, len) Str_Compare(str1, str2, len)
#ifndef STR_COMPARE
	#include "string.h"
	#define STR_COMPARE(str1, str2, len) memcmp(str1, str2, len)
#endif

//------------------------------------------------------------------
#ifndef offsetof
	//#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)// 获取"MEMBER成员"在"结构体TYPE"中的位置偏移
	#include <stddef.h>
#endif

#ifndef __GNUC__
	// 根据"结构体(type)变量"中的"域成员变量(member)的指针(ptr)"来获取指向整个结构体变量的指针
	#define container_of(ptr, type, member)  ((type*)((char*)ptr - offsetof(type, member)))
	// 此宏定义原文为 GNU C 所写，如下，有些编译器只支持 ANSI C /C99 的，所以作以上修改
#else
	#define container_of(ptr, type, member) ({          \
		const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
		(type *)( (char *)__mptr - offsetof(type,member) );})
#endif
//------------------------------------------------------------------

#include "usart3.h"
#define shell_print __printf

unsigned char Str_Compare(char * str1, char * str2, unsigned short int len);
float String_To_Float(char *array);\
char* Get_Cli_Str(void);

#endif

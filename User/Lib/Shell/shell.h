#ifndef __SHELL_H
#define __SHELL_H

/// 命令对应的函数类型，至于为什么输入设计为 void *,我不记得了
typedef void (*cmd_callback_func)(char * arg);


typedef struct _shell_command
{
    unsigned int id;  ///< 对 name 字符串进行压缩得到的 ID 号，匹配数字比字符串效率高。
    char *name;       ///< 记录每条命令字符串的内存地址
    cmd_callback_func func;    ///< 记录命令回调函数对应的内存地址
    struct _shell_command *next; ///< 命令索引接入点，用链表对命令作集合
}Shell_command_t;


//注册命令时调用此函数
#define Shell_Register_Command(name, func) \
{ \
	static Shell_command_t new_cmd = {0}; \
	_Shell_Register(&shell_cmd_root, &new_cmd, name, func); \
}

void _Shell_Register(Shell_command_t * root, Shell_command_t * new_cmd, char * cmd_name, cmd_callback_func cmd_func);
void Shell_Init(void);
void Shell_Command_Parse(char * arg);

int Shell_Split_String(char * str ,char ** argv ,int maxread);

#endif

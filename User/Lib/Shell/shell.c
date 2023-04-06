#include "shell.h"
#include "shellio.h"
#include "user_commands.h"

union cmd_id
{
    struct
    { // 命令号分为以下五个部分
        unsigned int num : 8;        ///< 命令序号
        unsigned int sum : 8;        ///< 命令字符的总和
        unsigned int len : 8;        ///< 命令字符的长度，8 bit ，即命令长度不能超过31个字符
        unsigned int first_char : 8; ///< 命令字符的第一个字符
    } part;

    unsigned int id; ///< 由此合并为 32 位的命令码
};

Shell_command_t shell_cmd_root;


/**
  * @author   Bashpow
  * @brief    新命令插入记录
  * @author   Bashpow
  * @param    root     : 检索根，链表头节点
  * @param    newcmd   : 新命令控制块
  * @return   成功返回 0
*/
static int Shell_Insert_Cmd(Shell_command_t * root , Shell_command_t * newcmd)
{
	Shell_command_t *node = root ;

	while( node->next != NULL )
	{
		if(newcmd->id == (node->next)->id)
			return -1;

		else if (newcmd->id < (node->next)->id)
			break;

		node = node->next;
	}

	newcmd->next = node->next;
	node->next = newcmd;

	root->id ++;
	return 0;
}

/**
  * @author   Bashpow
  * @brief    注册一个命令号和对应的命令函数 
  * @note     前缀为 '_' 表示不建议直接调用此函数，通过相对于的宏定义来调用
  * @param    root     : 命令链表头
  * @param    new_cmd   : 命令控制块对应的指针
  * @param    cmd_name : 命令名
  * @param    cmd_func : 命令名对应的执行函数
  * @return   无
*/
void _Shell_Register(Shell_command_t * root, Shell_command_t * new_cmd, char * cmd_name, cmd_callback_func cmd_func)
{
	union cmd_id new_cmd_id;
	unsigned int tmp_sum = 0;
	unsigned int tmp_len = 0;

	for(; cmd_name[tmp_len] != '\0'; tmp_len++)
	{
		tmp_sum += cmd_name[tmp_len];
	}

	new_cmd_id.part.num = root->id + 1;
	new_cmd_id.part.sum = tmp_sum;
	new_cmd_id.part.len = tmp_len;
	new_cmd_id.part.first_char = cmd_name[0];

	new_cmd->id = new_cmd_id.id;
	new_cmd->name = cmd_name;
	new_cmd->func = cmd_func;
	new_cmd->next = NULL;

	Shell_Insert_Cmd(root, new_cmd); //插入至命令链表
}

/**
  * @author   Bashpow
  * @brief    解析命令
  * @param    arg  : 命令行内存
  * @return   无
*/
void Shell_Command_Parse(char * arg)
{
	Shell_command_t *node = &shell_cmd_root ;
	union cmd_id node_id;

	while( node->next != NULL )
	{
		node = node->next;
		node_id.id = node->id;

		if(STR_COMPARE(node->name, arg, node_id.part.len))
		{
			if ((arg[node_id.part.len] == '\0') || (arg[node_id.part.len] == ' '))
			{
				shell_print("rov> %s\r\n", arg);
				node->func(arg); //执行回调函数
				shell_print("\r\n");
				return;
			}
		}
	}

}

/**
  * @author   Bashpow
  * @brief    eg.把 "abc bc cdef xyz" 拆分为 char*argv[] = {"abc","bc","cdef","xyz"};
  * @note     运行过后字符串缓冲区内容将被修改
  * @param    str    : 命令字符串后面所跟参数缓冲区指针
  * @param    argv   : 数据转换后缓存地址
  * @param    maxread: 最大读取数（一般为argv的个数）
  * @return   最终读取参数个数输出
*/
int Shell_Split_String(char * str ,char ** argv ,int maxread)
{
	int argc = 0;

	for (; ' ' == *str; ++str)
		; //跳过空格

	for (; *str && argc < maxread; ++argc, ++argv) //字符不为 ‘\0' 的时候
	{

		for (*argv = str; ' ' != *str && *str; ++str)
			; //记录这个参数，然后跳过非空字符

		for (; ' ' == *str; *str++ = '\0')
			; //每个参数加字符串结束符，跳过空格
	}

	return argc;
}


/**
  * @author   Bashpow
  * @brief    显示所有注册了的命令
  * @param    arg  : 命令行内存
  * @return   无
*/
#ifndef VERSION
	#define VERSION "-"
#endif
static void Infantry_Version(char * arg)
{
	shell_print("\r\n");
	shell_print("    __  ____    __       _____                          __  __              \r\n");
	shell_print("   / / / / /   / /      / ___/__  ______  ___  _____   / / / /__  _________ \r\n");
	shell_print("  / /_/ / /   / /       \\__ \\/ / / / __ \\/ _ \\/ ___/  / /_/ / _ \\/ ___/ __ \r\n");
	shell_print(" / __  / /___/ /___    ___/ / /_/ / /_/ /  __/ /     / __  /  __/ /  / /_/ /\r\n");
	shell_print("/_/ /_/_____/_____/   /____/\\__,_/ .___/\\___/_/     /_/ /_/\\___/_/   \\____/ \r\n");
	shell_print("                                /_/                                         \r\n");
	shell_print("\r\n");
	shell_print("\r\n");
	shell_print("version:\t%s\r\n", VERSION);
}


/**
  * @author   Bashpow
  * @brief    显示所有注册了的命令
  * @param    arg  : 命令行内存
  * @return   无
*/
static void Shell_List_Cmd(char * arg)
{
	Shell_command_t *node = &shell_cmd_root ;
	shell_print("command----------\r\n");
	while( node->next != NULL )
	{
		node = node->next;
		shell_print("id:%d, name:%s\r\n", node->id, node->name);
	}
	shell_print("-----------------\r\n");
}

/**
  * @author   Bashpow
  * @brief    shell 初始化,注册几条基本的命令，注册用户命令。不建议不初始化。
  * @return   无
*/
void Shell_Init(void)
{
	{
		shell_cmd_root.id = 0;
		shell_cmd_root.func = NULL;
		shell_cmd_root.next = NULL;
	}
	
	/* 注册一些基本命令 */
	Shell_Register_Command("version",       Infantry_Version);
	Shell_Register_Command("command-list" , Shell_List_Cmd);
	Shell_Register_Command("?" ,            Shell_List_Cmd);

	/* 注册用户命令 */
	User_Commands_Init();
}

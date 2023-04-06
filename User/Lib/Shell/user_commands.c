#include "user_commands.h"
#include "shell.h"
#include "shellio.h"
extern Shell_command_t shell_cmd_root;

//用户头文件包含

//变量定义

//函数声明
static void Module_Status(char * arg);
static void Gyroscope_Data(char * arg);
static void Gyro_Reset(char * arg);
static void Pid_Show(char * arg);
static void Pid_Set(char * arg);
static void Pid2_Show(char * arg);
static void Pid2_Set(char * arg);
static void Pid3_Show(char * arg);
static void Pid3_Set(char * arg);

//用户命令初始化
void User_Commands_Init(void)
{
	//变量初始化

	//用户命令注册
	Shell_Register_Command("module-status" , Module_Status);
	Shell_Register_Command("gyroscope-data" , Gyroscope_Data);
	Shell_Register_Command("gyro-reset" , Gyro_Reset);
	Shell_Register_Command("pid-show" , Pid_Show);
	Shell_Register_Command("pid-set" , Pid_Set);
	Shell_Register_Command("pid2-show" , Pid2_Show);
	Shell_Register_Command("pid2-set" , Pid2_Set);
	Shell_Register_Command("pid3-show" , Pid3_Show);
	Shell_Register_Command("pid3-set" , Pid3_Set);
}

// #define MODULE_STATUS_PRINT(module) {ONLINE_STATUS_PRINT(module);TIMEOUT_PRINT(module);ERROR_PRINT(module);}
// #define ONLINE_STATUS_PRINT(module) { if(Get_Module_Online_State(module)){shell_print("ON-line\t");}else{shell_print("OFF-line\t");} }
// #define TIMEOUT_PRINT(module) { shell_print("Cumulative timeout: %.1fs\t", Get_Module_Time_Out_Time_Second(module)); }
// #define ERROR_PRINT(module) { shell_print("Error time: %d\r\n", Get_Module_Err_Time(module)); }
static void Module_Status(char * arg)
{
	shell_print("Module----------\r\n");

	// shell_print("host: ");
	// MODULE_STATUS_PRINT(0);

	// shell_print("gyro: ");
	// MODULE_STATUS_PRINT(1);

	// shell_print("sht10: ");
	// MODULE_STATUS_PRINT(2);

	shell_print("----------------\r\n");
}

static void Gyroscope_Data(char * arg)
{
	// if(1)
	// {
	// 	shell_print("roll:%.2f, pitch:%.2f, yaw:%.2f", gyro_data->angle.roll_x, gyro_data->angle.pitch_y, gyro_data->angle.yaw_z);
	// }
	// else
	// {
	// 	shell_print("Gyroscope offline!");
	// }
	// shell_print("\r\n");
	shell_print("TODO\r\n");
}



static void Gyro_Reset(char * arg)
{
	// Create_Gyro_Calibration_Thread();
	// shell_print("Try create Gyro Calibration Thread...\r\n");
	shell_print("TODO\r\n");
}

float easy_pid_p, easy_pid_i, easy_pid_d;
static void Pid_Show(char * arg)
{
	shell_print("p:%.6f\ti:%.6f\td:%.6f\r\n", easy_pid_p, easy_pid_i, easy_pid_d);
}
static void Pid_Set(char * arg)
{
	/*
	char * argv[4];
	int argc = Shell_Split_String((char*)arg,argv,4);
	shell_print("get %d parameter\r\n", argc);
	for(int i = 0 ; i < argc ; ++i)
		shell_print("argv[%d]: \"%s\"\r\n",i,argv[i]);
	*/
	char * argv[3];
	int argc = Shell_Split_String(arg, argv, 3);
	switch(argv[1][0])
	{
		case 'p':
			easy_pid_p = String_To_Float(argv[2]);
			shell_print("EASY PID p set %.6f", easy_pid_p);
			break;

		case 'i':
			easy_pid_i = String_To_Float(argv[2]);
			shell_print("EASY PID i set %.6f", easy_pid_i);
			break;

		case 'd':
			easy_pid_d = String_To_Float(argv[2]);
			shell_print("EASY PID d set %.6f", easy_pid_d);
			break;

		default:
			shell_print("Error!");
			break;
	}
	shell_print("\r\n");
}

float easy_pid2_p, easy_pid2_i, easy_pid2_d;
static void Pid2_Show(char * arg)
{
	shell_print("p:%.6f\ti:%.6f\td:%.6f\r\n", easy_pid2_p, easy_pid2_i, easy_pid2_d);
}
static void Pid2_Set(char * arg)
{
	char * argv[3];
	int argc = Shell_Split_String(arg, argv, 3);
	switch(argv[1][0])
	{
		case 'p':
			easy_pid2_p = String_To_Float(argv[2]);
			shell_print("EASY PID2 p set %.6f", easy_pid2_p);
			break;

		case 'i':
			easy_pid2_i = String_To_Float(argv[2]);
			shell_print("EASY PID2 i set %.6f", easy_pid2_i);
			break;

		case 'd':
			easy_pid2_d = String_To_Float(argv[2]);
			shell_print("EASY PID2 d set %.6f", easy_pid2_d);
			break;

		default:
			shell_print("Error!");
			break;
	}
	shell_print("\r\n");
}

float easy_pid3_p, easy_pid3_i, easy_pid3_d;
static void Pid3_Show(char * arg)
{
	shell_print("p:%.6f\ti:%.6f\td:%.6f\r\n", easy_pid3_p, easy_pid3_i, easy_pid3_d);
}
static void Pid3_Set(char * arg)
{
	char * argv[3];
	int argc = Shell_Split_String(arg, argv, 3);
	switch(argv[1][0])
	{
		case 'p':
			easy_pid3_p = String_To_Float(argv[2]);
			shell_print("EASY PID3 p set %.6f", easy_pid3_p);
			break;

		case 'i':
			easy_pid3_i = String_To_Float(argv[2]);
			shell_print("EASY PID3 i set %.6f", easy_pid3_i);
			break;

		case 'd':
			easy_pid3_d = String_To_Float(argv[2]);
			shell_print("EASY PID3 d set %.6f", easy_pid3_d);
			break;

		default:
			shell_print("Error!");
			break;
	}
	shell_print("\r\n");
}

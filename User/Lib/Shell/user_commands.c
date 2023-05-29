#include "user_commands.h"
#include "shell.h"
#include "shellio.h"
extern Shell_command_t shell_cmd_root;

//用户头文件包含
#include "math2.h"
#include "monitor_task.h"
#include "can1_device.h"
#include "can2_device.h"
#include "shoot_task.h"
#include "gimbal_task.h"
#include "super_capacitor_task.h"
#include "externel_gyroscope_task.h"
//#include "autoaim.h"（希望有一天电控和视觉能开始联调）

//变量定义
static const Motor_Measure_t* shell_chassis_motor;
static const Motor_Measure_t* shell_shooter_wave_motor; //波轮电机数据
static const Motor_Measure_t* shell_gimbal_motor; //云台电机数据
static const Motor_Measure_t* shell_shooter_fric_motor;
static const Super_Capacitor_t* super_cap_data;
static const Wt61c_Data_t* wt61c_data;
//static const Auto_aim_t* auto_aim_msg;（希望有一天电控和视觉能开始联调）

//函数声明
static void Module_Status(char * arg);
static void Motors_Data(char * arg);
static void Gimbal_Data(char * arg);
//static void Autoaim_Data(char * arg);
static void Super_Cap_Data(char * arg);
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
	shell_chassis_motor = Get_Can1_Feedback_Data();
	shell_shooter_wave_motor = Get_Wave_Motor_Paresed_Data();
	shell_gimbal_motor = Get_Gimbal_Parsed_FeedBack_Data();
	shell_shooter_fric_motor = Get_Shooter_Parsed_FeedBack_Data();
	super_cap_data = Get_SuperCapacitor_Date();
	wt61c_data = Get_Gyroscope_Data_t();
	//auto_aim_msg = Get_Auto_Aim_Msg();（希望有一天电控和视觉能开始联调）
	
	//用户命令注册
	Shell_Register_Command("module-status" , Module_Status);
	Shell_Register_Command("gyroscope-data" , Gyroscope_Data);
	Shell_Register_Command("motors-data" , Motors_Data);
	Shell_Register_Command("gimbal-data" , Gimbal_Data);
	Shell_Register_Command("super-cap-data" , Super_Cap_Data);
	Shell_Register_Command("gyro-reset" , Gyro_Reset);
	Shell_Register_Command("pid-show" , Pid_Show);
	Shell_Register_Command("pid-set" , Pid_Set);
	Shell_Register_Command("pid2-show" , Pid2_Show);
	Shell_Register_Command("pid2-set" , Pid2_Set);
	Shell_Register_Command("pid3-show" , Pid3_Show);
	Shell_Register_Command("pid3-set" , Pid3_Set);
}


#define ONLINE_STATUS_PRINT(module) { if(Get_Module_Online_State(module)){shell_print("ON-line\r\n");}else{shell_print("OFF-line\r\n");} }
static void Module_Status(char * arg)
{
	shell_print("Module----------\r\n");

	shell_print("remote control:\t");
	ONLINE_STATUS_PRINT(0);

	shell_print("chassis motor:\t");
	ONLINE_STATUS_PRINT(1);

	shell_print("gimbal motor:\t");
	ONLINE_STATUS_PRINT(2);

	shell_print("shooter motor:\t");
	ONLINE_STATUS_PRINT(3);

	shell_print("auto aim:\t");
	ONLINE_STATUS_PRINT(4);

	shell_print("judge system:\t");
	ONLINE_STATUS_PRINT(5);

	shell_print("super cap:\t");
	ONLINE_STATUS_PRINT(6);

	shell_print("gyroscope:\t");
	ONLINE_STATUS_PRINT(7);

	shell_print("----------------\r\n");
}

#define PRINT_MOTOR_C620_DATA(name, data) shell_print("%s\tangle: %d, speed: %drpm, current: %.1fA, temperate: %dC\r\n", name, data.mechanical_angle, data.speed_rpm, (((float)(data.actual_torque_current))*20.0f/16384.0f), data.temperate);
#define PRINT_MOTOR_GM6020_DATA(name, data) shell_print("%s\tangle: %d, speed: %drpm, current: %d, temperate: %dC\r\n", name, data.mechanical_angle, data.speed_rpm, data.actual_torque_current, data.temperate);
#define PRINT_MOTOR_C615_DATA(name, data) shell_print("%s\tangle: %d, speed: %drpm, torque: %d\r\n", name, data.mechanical_angle, data.speed_rpm, data.actual_torque_current);
static void Motors_Data(char * arg)
{
	PRINT_MOTOR_C620_DATA("chassis motor1", shell_chassis_motor[0]);
	PRINT_MOTOR_C620_DATA("chassis motor2", shell_chassis_motor[1]);
	PRINT_MOTOR_C620_DATA("chassis motor3", shell_chassis_motor[2]);
	PRINT_MOTOR_C620_DATA("chassis motor4", shell_chassis_motor[3]);
	PRINT_MOTOR_GM6020_DATA("yaw motor", shell_gimbal_motor[0]);
	PRINT_MOTOR_GM6020_DATA("pitch motor", shell_gimbal_motor[1]);
	PRINT_MOTOR_C620_DATA("wave motor", shell_shooter_wave_motor[0]);
	PRINT_MOTOR_C620_DATA("fric motor right",shell_shooter_fric_motor[0]);
	PRINT_MOTOR_C620_DATA("fric motor left",shell_shooter_fric_motor[1]);
	shell_print("\r\n");
}

static void Gimbal_Data(char * arg)
{
	PRINT_MOTOR_GM6020_DATA("yaw motor", shell_gimbal_motor[0]);
	PRINT_MOTOR_GM6020_DATA("pitch motor", shell_gimbal_motor[1]);
	shell_print("\r\n");
}

static void Gyroscope_Data(char * arg)
{
	if(Get_Module_Online_State(7))
	{
		shell_print("roll:%.2f, pitch:%.2f, yaw:%.2f", wt61c_data->angle.roll_x, wt61c_data->angle.pitch_y, wt61c_data->angle.yaw_z);
	}
	else
	{
		shell_print("Gyroscope offline!");
	}
	shell_print("\r\n");
}

static void Super_Cap_Data(char * arg)
{
	if(Get_Module_Online_State(6))
	{
		shell_print("input voltage:%.2f, cap voltage:%.2f, input current:%.2f, target power:%.2f", super_cap_data->input_voltage, super_cap_data->cap_voltage, super_cap_data->input_current, super_cap_data->target_power);
	}
	else
	{
		shell_print("Super capacitor offline!");
	}
	shell_print("\r\n");
}


static void Gyro_Reset(char * arg)
{
	// Create_Gyro_Calibration_Thread();
	// shell_print("Try create Gyro Calibration Thread...\r\n");
	shell_print("TODO NOLLFINISH\r\n");
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

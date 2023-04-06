#include "referee_system.h"
#include "console_print.h"
#include "crc_check.h"
#include "math2.h"
#include "monitor_task.h"
#include "usart3.h"

/* 裁判系统调试宏定义 */
#if 1
#define DEBUG_LOG debug_log
#define DEBUG_ERROR debug_error
#else
#define DEBUG_LOG(format, arg) /* DEBUG OFF */
#define DEBUG_ERROR(err)	   /* DEBUG OFF */
#endif

#if 0
#define JUDGE_ARRAY DEBUG_ARRAY
#else
#define JUDGE_ARRAY(name, data, len)
#endif

#define DATA_LEN_CHECK(datalen, correct_len, err) \
	if (data_len != correct_len)                  \
	{                                             \
		DEBUG_ERROR(err);                         \
		return 0;                                 \
	}

//函数声明
void Find_All_A5(uint8_t *get_data, uint16_t data_len, uint8_t *r_position, uint8_t *r_a5_length);
uint8_t Analysis_Frame_Header(uint8_t *get_data, uint16_t *r_data_length, uint8_t *r_seq);
uint8_t Check_Package_Crc16(uint8_t *get_data, uint16_t data_len);
uint16_t Analysis_Cmd_Id(uint8_t *get_data);
uint8_t Analysis_Data(uint8_t *get_data, uint16_t data_len);

static uint8_t Analysis_Power_Heat_Data(uint8_t *data_package, uint16_t data_len);
static uint8_t Analysis_Shoot_Data(uint8_t *data_package, uint16_t data_len);
static uint8_t Analysis_Game_Robot_Status(uint8_t *data_package, uint16_t data_len);

//裁判系统数据定义
static Judge_data_t judge_data;

///<返回裁判系统数据结构体地址
const Judge_data_t *Get_Referee_Data(void)
{
	return &judge_data;
}

/*
  函数名：Analysis_Judge_System
  描述  ：解析裁判系统数据
  参数  ：get_data需要解析的帧头数据，data_len数据长度
  返回值：0--解析失败 1--解析成功
*/
uint8_t Parse_Refere_System_Data(uint8_t *get_data, uint16_t data_len)
{
	uint8_t a5_position[8];	 // 0xA5的位置
	uint8_t a5_number = 0;	 // 0xA5的个数（数据包个数）
	uint16_t data_length[8]; //每个data数据包的长度

	//寻找帧头
	Find_All_A5(get_data, data_len, a5_position, &a5_number);
	//解析所有数据包
	for (uint8_t i = 0; i < a5_number; i++)
	{
		//解析帧头数据
		if (Analysis_Frame_Header(&get_data[(a5_position[i])], &data_length[i], NULL) == 0)
		{
			// debug_log("Analysis No.%d frame header error.", i);
			// Console.print("head_done%f\r\n", i);
			continue;
		}

		//整包CRC16校验
		if (Check_Package_Crc16(&get_data[(a5_position[i])], (data_length[i] + 9)) == 0)
		{
			// debug_log("CRC16 check No.%d fail.", i);
			// Console.print("CRC_done\r\n");

			continue;
		}

		//解析该数据包
		if (Analysis_Data(&get_data[(a5_position[i])], data_length[i]) == 0)
		{
			// debug_log("Analysis No.%d data fail.", i);
			// Console.print("par\r\n");

			continue;
		}

		// Console.print("x%d len:%d p:%d id:%d\r\n", i, data_length[i], a5_position[i], Analysis_Cmd_Id(&get_data[(a5_position[i])]));
	}

	return 1;
}

/*
  函数名：Find_All_A5
  描述  ：找到所有帧头的头
  参数  ：get_data需要解析的帧头数据，data_len数据长度，r_position 0xA5位置数组，r_a5_length返回数组长度的长度
  返回值：无
*/
void Find_All_A5(uint8_t *get_data, uint16_t data_len, uint8_t *r_position, uint8_t *r_a5_length)
{
	*r_a5_length = 0;

	for (uint16_t i = 0; i < data_len; i++)
	{
		if (get_data[i] == 0xA5)
		{
			r_position[*r_a5_length] = i;

			(*r_a5_length)++;
		}
	}
}

/*
  函数名：Analysis_Frame_Header
  描述  ：解析帧头数据
  参数  ：get_data需要解析的数据包，r_data_length返回数据帧中 data 的长度，r_seq返回seq值
  返回值：0--解析失败 1--解析成功
*/
uint8_t Analysis_Frame_Header(uint8_t *get_data, uint16_t *r_data_length, uint8_t *r_seq)
{
	JUDGE_ARRAY("head", get_data, 5);

	//判断帧头的头是否为0xA5
	if (get_data[0] != 0xA5)
	{
		DEBUG_ERROR(501);
		return 0;
	}

	//帧头CRC8校验
	if (Verify_CRC8_Check_Sum(get_data, 5) == 0)
	{
		DEBUG_ERROR(502);
		return 0;
	}

	//解析data的长度
	if (r_data_length != NULL)
	{
		*r_data_length = get_data[1] | (get_data[2] << 8);
	}

	//解析seq(包序号)
	if (r_seq != NULL)
	{
		*r_seq = get_data[3];
	}

	return 1;
}

/*
  函数名：Check_Package_Crc16
  描述  ：一个数据包进行CRC16校验
  参数  ：get_data需要解析的数据包， data_len包长度
  返回值：0--解析失败 1--解析成功
*/
uint8_t Check_Package_Crc16(uint8_t *get_data, uint16_t data_len)
{
	return Verify_CRC16_Check_Sum(get_data, data_len);
}

/*
  函数名：Analysis_Cmd_Id
  描述  ：解析命令ID数据
  参数  ：get_data需要解析的数据包
  返回值：cmd_id
*/
uint16_t Analysis_Cmd_Id(uint8_t *get_data)
{
	return get_data[5] + (get_data[6] << 8);
}

/*
  函数名：Analysis_Data
  描述  ：解析data数据
  参数  ：get_data需要解析的数据包, data_len data的长度
  返回值：0--解析失败 1--解析成功
*/
uint8_t Analysis_Data(uint8_t *get_data, uint16_t data_len)
{
	uint16_t cmd_id = get_data[5] | (get_data[6] << 8);

	switch (cmd_id)
	{
	//实时功率热量数据
	case Power_Heat_Data:
	{
		return Analysis_Power_Heat_Data(&get_data[7], data_len);
		// break;
	}
	//实时射击信息
	case Shoot_Data:
	{
		return Analysis_Shoot_Data(&get_data[7], data_len);
		// break;
	}
	//实时比赛机器人状态
	case Game_Robot_Status:
	{
		return Analysis_Game_Robot_Status(&get_data[7], data_len);
		// break;
	}

	case BUFF_MUSK_CMD_ID:
	{
		memcpy(&judge_data.ext_buff_musk_t,&get_data[7], data_len);
	}
	case BULLET_REMAINING_CMD_ID:
	{
		// memcpy(&judge_data.ext_bullet_remaining_t.bullet_remaining_num,&get_data[7], sizeof(judge_data.ext_bullet_remaining_t));
		memcpy(&judge_data.ext_bullet_remaining_t.bullet_remaining_num, &get_data[7], data_len);
	}
	}

	return 0x0A;
}

//解析实时功率热量数据
static uint8_t Analysis_Power_Heat_Data(uint8_t *data_package, uint16_t data_len)
{
	DATA_LEN_CHECK(data_len, 16, 503);

	judge_data.power_heat_data.chassis_power = Hex4_To_Float1(&data_package[4]);
	judge_data.power_heat_data.chassis_power_buffer = Uint8_t_Array_To_Uint16_t(&data_package[8]);
	judge_data.power_heat_data.shooter_id1_17mm_cooling_heat = Uint8_t_Array_To_Uint16_t(&data_package[10]);
	// debug_showdata2("chassis_power", judge_data.power_heat_data.chassis_power);
	// Console.print("chassis_power", judge_data.power_heat_data.shooter_id1_17mm_cooling_heat);
	return 1;
}

/**
 * @note	unfinished
 * @brief	解析实时射击信息
 * @return	解析结果：1成功解析；0解析失败（长度校验不通过）
 * */
static uint8_t Analysis_Shoot_Data(uint8_t *data_package, uint16_t data_len)
{
	DATA_LEN_CHECK(data_len, 7, 504);

	memcpy(&judge_data.shoot_data, data_package, 7);

	/**
	 * @note	unfinished
	 */
	//  Shooter_Friction_Speed_Limit();///<限制子弹发射速度函数，待完善，待解析裁判系统数据
	debug_showdata2("bullet_speed", judge_data.shoot_data.bullet_speed);

	return 1;
}

/**
 * @brief		解析比赛机器人状态
 * @param[in]	数据包数组首地址（对应偏移量为0）
 * @param[in]	数据长度
 * @return		成功解析：1 &数据长度不符，返回0并且打印错误数据505
 *  */
static uint8_t Analysis_Game_Robot_Status(uint8_t *data_package, uint16_t data_len)
{
	DATA_LEN_CHECK(data_len, 27, 505);

	judge_data.game_robot_status.robot_id = data_package[0];

	//机器人 42mm 枪口每秒冷却值
	judge_data.game_robot_status.shooter_id1_42mm_cooling_rate = Uint8_t_Array_To_Uint16_t(&data_package[18]);

	//机器人 42mm 枪口热量上限
	judge_data.game_robot_status.shooter_id1_42mm_cooling_limit = Uint8_t_Array_To_Uint16_t(&data_package[20]);

	//机器人 42mm 枪口上限速度 单位 m/s
	judge_data.game_robot_status.shooter_id1_42mm_speed_limit = Uint8_t_Array_To_Uint16_t(&data_package[22]);
	/**
	 * @note	unfinished
	 */
	// Shooter_Friction_Speed_Base_Limit(judge_data.game_robot_status.shooter_id1_17mm_speed_limit); //射速变更时，调整摩擦轮速度

	//底盘功率上限
	judge_data.game_robot_status.chassis_power_limit = Uint8_t_Array_To_Uint16_t(&data_package[24]);

	judge_data.game_robot_status.mains_power_shooter_output = (data_package[26] & 0x04) >> 2;

	// //模拟发射机构断电
	// if(judge_data.game_robot_status.mains_power_shooter_output)
	// {
	// 	POWER1_CTRL_ON;
	// 	POWER2_CTRL_ON;
	// }
	// else
	// {
	// 	POWER1_CTRL_OFF;
	// 	POWER2_CTRL_OFF;
	// 	Fric_Reset(); //重置摩擦轮
	// }

	debug_showdata1("sout", judge_data.game_robot_status.mains_power_shooter_output);
	debug_showdata1("gggglllll", judge_data.game_robot_status.chassis_power_limit);

	return 1;
}

//判断42mm发射机构是否超热量
uint8_t Is_Id1_17mm_Excess_Heat(const Judge_data_t *judge_data)
{
	if (judge_data->power_heat_data.shooter_id1_42mm_cooling_heat == 65535 || Get_Module_Online_State(5) == 0)
	{
		return 0;
	}
	if (judge_data->power_heat_data.shooter_id1_42mm_cooling_heat <= (judge_data->power_heat_data.shooter_id1_42mm_cooling_heat + 12))
	{
		return 1;
	}
	return 0;
}

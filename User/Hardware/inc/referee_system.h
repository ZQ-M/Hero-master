#ifndef __JUDGE_SYSTEM_H
#define __JUDGE_SYSTEM_H

#include "crc_check.h"
#include "math2.h"
#include "monitor_task.h"
#include "sys.h"
#include "usart3.h"

// uint8_t Parse_Refere_System_Data(uint8_t *get_data, uint16_t data_len);

// uart 通信配置，波特率 115200, 数据位 8, 停止位 1，检验位无，流控制无。
// --------------------------------------------------------------------------------
//| FrameHeader(5-Byte) | CmdID(2-Byte) | Data(n-Byte) | FrameTail(2-Byte, CRC16) |
// --------------------------------------------------------------------------------
// crc8 generator polynomial:G(x)=x8+x5+x4+1

/* 命令ID */
typedef enum
{
	Game_Status = 0x0001,			 //比赛状态数据，1Hz 周期发送(0x0001)
	Game_Result = 0x0002,			 //比赛结果数据，比赛结束后发送(0x0002)
	Game_Robot_HP = 0x0003,			 //比赛机器人血量数据，1Hz 周期发送(0x0003)
	Game_Robot_Status = 0x0201,		 //比赛机器人状态：0x0201。发送频率：10Hz
	Power_Heat_Data = 0x0202,		 //实时功率热量数据，50Hz 周期发送(0x0202)
	BUFF_MUSK_CMD_ID = 0x0204,		 //增益数据
	Shoot_Data = 0x0207,			 //实时射击信息。发送频率：射击后发送(0x0207)
	BULLET_REMAINING_CMD_ID = 0x0208 //子弹剩余数量
} Cmd_ID;

typedef struct
{
	__packed struct
	{
		uint8_t robot_id;
		uint8_t robot_level;
		uint16_t remain_HP;
		uint16_t max_HP;
		uint16_t shooter_id1_17mm_cooling_rate;
		uint16_t shooter_id1_17mm_cooling_limit;
		uint16_t shooter_id1_17mm_speed_limit;
		uint16_t shooter_id2_17mm_cooling_rate;
		uint16_t shooter_id2_17mm_cooling_limit;
		uint16_t shooter_id2_17mm_speed_limit;
		uint16_t shooter_id1_42mm_cooling_rate;
		uint16_t shooter_id1_42mm_cooling_limit;
		uint16_t shooter_id1_42mm_speed_limit;
		uint16_t chassis_power_limit;
		uint8_t mains_power_gimbal_output : 1;
		uint8_t mains_power_chassis_output : 1;
		uint8_t mains_power_shooter_output : 1;
	} game_robot_status;

	__packed struct
	{
		uint16_t chassis_volt;
		uint16_t chassis_current;
		float chassis_power;
		uint16_t chassis_power_buffer;
		uint16_t shooter_id1_17mm_cooling_heat;
		uint16_t shooter_id2_17mm_cooling_heat;
		uint16_t shooter_id1_42mm_cooling_heat;
	} power_heat_data;

	__packed struct
	{
		uint8_t bullet_type;
		uint8_t shooter_id;
		uint8_t bullet_freq;
		float bullet_speed;
	} shoot_data;
	__packed struct
	{
		uint8_t bullet_remaining_num;
	} ext_bullet_remaining_t;
	__packed struct // 0x0204
	{
		uint8_t power_rune_buff;
	} ext_buff_musk_t;

} Judge_data_t;
const Judge_data_t *Get_Referee_Data(void);
// Judge_data_t* Get_Referee_Data(void);///<返回裁判系统数据结构体地址以供外部调用（有错误，声明被识别为定义）
uint8_t Is_Id1_17mm_Excess_Heat(const Judge_data_t *judge_data);
uint8_t Parse_Refere_System_Data(uint8_t *get_data, uint16_t data_len);
void Find_All_A5(uint8_t *get_data, uint16_t data_len, uint8_t *r_position, uint8_t *r_a5_length);
#endif

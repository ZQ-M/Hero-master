#ifndef __JUDGE_SYSTEM_H
#define __JUDGE_SYSTEM_H

#include "sys.h"

// uart 通信配置，波特率 115200, 数据位 8, 停止位 1，检验位无，流控制无。
// --------------------------------------------------------------------------------
//| FrameHeader(5-Byte) | CmdID(2-Byte) | Data(n-Byte) | FrameTail(2-Byte, CRC16) |
// --------------------------------------------------------------------------------
//crc8 generator polynomial:G(x)=x8+x5+x4+1

/**
 * @brief 裁判系统条目（单个）
 * 命令ID---数据长度---存储空间首地址
 * 
 */
struct Judge_System_Connect_Item {
	uint16_t cmd_id;
	uint16_t data_len;
	void* save_space;
	void(*Callback)(unsigned short);
};

/**
 * @brief 裁判系统条目链表节点
 * 
 */
typedef struct _Judge_System_Connect_Item_Node {
	struct Judge_System_Connect_Item data;
	struct _Judge_System_Connect_Item_Node* next;
}Judge_System_Connect_Item_Node;

void _Judge_System_Connect_Register(Judge_System_Connect_Item_Node *root, Judge_System_Connect_Item_Node *new_item, const uint16_t cmd_id, const uint16_t data_len, void *save_space, void(*Callback)(unsigned short));

/**
 * @brief 裁判系统条目注册
 * 
 */
#define Judge_System_Connect_Item_Register(cmd_id, data_len, save_space, callback_func) \
{ \
	static Judge_System_Connect_Item_Node new_connect_item = {0}; \
	_Judge_System_Connect_Register(&judge_system_connect_root, &new_connect_item, cmd_id, data_len, save_space, callback_func); \
}

typedef  struct
{
	// 弃用
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
	}game_robot_status;
	
	// 弃用
	__packed struct
	{
		uint16_t chassis_volt; 
		uint16_t chassis_current; 
		float chassis_power; 
		uint16_t chassis_power_buffer; 
		uint16_t shooter_id1_17mm_cooling_heat;
		uint16_t shooter_id2_17mm_cooling_heat;
		uint16_t shooter_id1_42mm_cooling_heat;
	}power_heat_data;

	// cmd_id 0x0102
	__packed struct
	{
		uint8_t supply_projectile_id;
		uint8_t supply_robot_id;
		uint8_t supply_projectile_step;
		uint8_t supply_projectile_num;
	} ext_supply_projectile_action_t;

	// cmd_id 0x0201
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
	} ext_game_robot_status_t;

	// cmd_id 0x0202
	__packed struct
	{
		uint16_t chassis_volt;
		uint16_t chassis_current;
		float chassis_power;
		uint16_t chassis_power_buffer;
		uint16_t shooter_id1_17mm_cooling_heat;
		uint16_t shooter_id2_17mm_cooling_heat;
		uint16_t shooter_id1_42mm_cooling_heat;
	} ext_power_heat_data_t;

	// cmd_id 0x0204
	__packed struct
	{
		uint8_t power_rune_buff;
	} ext_buff_t;

	// 0x0207
	__packed struct
	{
		uint8_t bullet_type;
		uint8_t shooter_id;
		uint8_t bullet_freq;
		float bullet_speed;
	} ext_shoot_data_t;

	// 弃用
	__packed struct
	{
		uint8_t bullet_type;
		uint8_t shooter_id;
		uint8_t bullet_freq;
		float bullet_speed;
	}shoot_data;

}Judge_data_t;

void Judge_System_Connect_List_Init(void);

const Judge_data_t* Get_Judge_Data(void);

uint8_t Analysis_Judge_System(u8 *get_data, u16 data_len);

u8 Is_Id1_17mm_Excess_Heat(const Judge_data_t* judge_data);

#endif

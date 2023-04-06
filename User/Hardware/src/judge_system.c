#include "judge_system.h"
#include "string.h"
#include "math2.h"
#include "crc_check.h"
#include "remoter_task.h"
#include "shooter_task.h"
#include "power_output.h"
#include "detect_task.h"

/* 裁判系统调试宏定义 */
#if 0
	#define JUDGE_LOG DEBUG_LOG
	#define JUDGE_ERROR DEBUG_ERROR
#else
	#define JUDGE_LOG(format, arg...) /* DEBUG OFF */
	#define JUDGE_ERROR(err) /* DEBUG OFF */
#endif

#if 0
	#define JUDGE_ARRAY DEBUG_ARRAY
#else
	#define JUDGE_ARRAY(name, data, len)
#endif

#define DATA_LEN_CHECK(datalen, correct_len, err)  if(data_len != correct_len){JUDGE_ERROR(err); return 0;}

//函数声明
void Find_All_A5(u8 *get_data, u16 data_len, u8 *r_position, u8 *r_a5_length);
uint8_t Analysis_Frame_Header(u8 *get_data, u16 *r_data_length, u8 *r_seq);
u8 Check_Package_Crc16(u8 *get_data, u16 data_len);
uint16_t Analysis_Cmd_Id(u8 *get_data);
uint8_t Analysis_Data(u8 *get_data, uint16_t data_len);

//裁判系统数据定义
static Judge_data_t judge_data;

/**
 * @brief 获取裁判系统数据
 * 
 * @return const Judge_data_t* 返回的数据头指针
 */
const Judge_data_t* Get_Judge_Data(void)
{
	return &judge_data;
}


// 裁判系统数据连接列表链表头节点
static Judge_System_Connect_Item_Node judge_system_connect_root;

/**
 * @brief 注册单个解析连接条目（在使用解析函数之前必须完成注册）
 * 
 * @param root 头节点指针
 * @param new_item 新条目指针，不允许为局部变量，全局变量或从内存池中申请的空间或其他
 * @param cmd_id 对应的命令ID
 * @param data_len 对应的数据帧长度
 * @param save_space 对应的数据存放空间（强烈建议使用官方裁判系统用户手册上提供的结构体）
 */
void _Judge_System_Connect_Register(Judge_System_Connect_Item_Node *root, Judge_System_Connect_Item_Node *new_item, const uint16_t cmd_id, const uint16_t data_len, void *save_space, void(*Callback)(unsigned short))
{
	// 头节点
	Judge_System_Connect_Item_Node *node = root;

	// 新数据项赋值
	new_item->data.cmd_id = cmd_id;
	new_item->data.data_len = data_len;
	new_item->data.save_space = save_space;
	new_item->data.Callback = Callback;
	new_item->next = NULL;

	// 将条目插入链表尾部
	while( node->next != NULL )
	{
		node = node->next;
	}
	node->next = new_item;
}

/**
 * @brief 初始化裁判系统数据连接列表，将要解析的数据都加入该初始化函数
 * 
 */
void Judge_System_Connect_List_Init(void)
{
	// 头节点初始化
	judge_system_connect_root.data.cmd_id = 0;
	judge_system_connect_root.data.data_len = 0;
	judge_system_connect_root.data.save_space = NULL;
	judge_system_connect_root.data.Callback = NULL;
	judge_system_connect_root.next = NULL;
 
	// 添加解析节点
	Judge_System_Connect_Item_Register(0x0102, 4, (void*)&judge_data.ext_supply_projectile_action_t, NULL);
	Judge_System_Connect_Item_Register(0x0201, 27, (void*)&judge_data.ext_game_robot_status_t, NULL);
	Judge_System_Connect_Item_Register(0x0202, 16, (void*)&judge_data.ext_power_heat_data_t, NULL);
	Judge_System_Connect_Item_Register(0x0204, 1, (void*)&judge_data.ext_buff_t, NULL);
	Judge_System_Connect_Item_Register(0x0207, 7, (void*)&judge_data.ext_shoot_data_t, NULL);
}


/**
 * @brief 解析裁判系统数据
 * 
 * @param get_data 需要解析的帧头数据
 * @param data_len 数据长度
 * @return uint8_t 0--解析失败 1--解析成功
 */
uint8_t Analysis_Judge_System(u8 *get_data, u16 data_len)
{
	u8 a5_position[8]; //0xA5的位置
	u8 a5_number = 0;  //0xA5的个数（数据包个数）
	u16 data_length[8];  //每个data数据包的长度
	
	//寻找帧头
	Find_All_A5(get_data, data_len, a5_position, &a5_number);
	
	//解析所有数据包
	for(u8 i=0; i<a5_number; i++)
	{
		//解析帧头数据
		if( Analysis_Frame_Header(&get_data[ (a5_position[i]) ], &data_length[i], NULL) == 0)
		{
			JUDGE_LOG("Analysis No.%d frame header error.", i);
			continue;
		}
		
		//整包CRC16校验
		if( Check_Package_Crc16(&get_data[ (a5_position[i]) ], (data_length[i]+9)) == 0 )
		{
			JUDGE_LOG("CRC16 check No.%d fail.", i);
			continue;
		}
		
		//解析该数据包
		if( Analysis_Data(&get_data[ (a5_position[i]) ], data_length[i]) == 0)
		{
			JUDGE_LOG("Analysis No.%d data fail.", i);
			continue;
		}
		
		// DEBUG_PRINT("x%d len:%d p:%d id:%d\r\n", i, data_length[i], a5_position[i], Analysis_Cmd_Id( &get_data[ (a5_position[i]) ] ) );
		
	}
	
	return 1;
}


/**
 * @brief 找到所有帧头的头
 * 
 * @param get_data 需要解析的帧头数据
 * @param data_len 数据长度
 * @param r_position 存放0xA5位置数组
 * @param r_a5_length 返回数组长度的长度
 */
void Find_All_A5(u8 *get_data, u16 data_len, u8 *r_position, u8 *r_a5_length)
{
	*r_a5_length = 0;
	
	for(u16 i=0; i<data_len; i++)
	{
		if(get_data[i] == 0xA5)
		{
			r_position[*r_a5_length] = i;
			
			(*r_a5_length) ++;
		}
	}
}


/**
 * @brief 解析帧头数据
 * 
 * @param get_data 需要解析的数据包
 * @param r_data_length 返回数据帧中 data 的长度
 * @param r_seq 返回seq值
 * @return uint8_t 0--解析失败 1--解析成功
 */
uint8_t Analysis_Frame_Header(u8 *get_data, u16 *r_data_length, u8 *r_seq)
{
	// JUDGE_ARRAY("head", get_data, 5);
	
	//判断帧头的头是否为0xA5
	if(get_data[0] != 0xA5)
	{
		JUDGE_ERROR(501);
		return 0;
	}
	
	//帧头CRC8校验
	if( Verify_CRC8_Check_Sum(get_data, 5) == 0)
	{
		JUDGE_ERROR(502);
		return 0;
	}
	
	//解析data的长度
	if(r_data_length != NULL)
	{
		*r_data_length = get_data[1] | (get_data[2] << 8);
	}
	
	//解析seq
	if(r_seq != NULL)
	{
		*r_seq = get_data[3];
	}
	
	return 1;
}


/**
 * @brief 一个数据包进行CRC16校验
 * 
 * @param get_data 需要解析的数据包
 * @param data_len 包长度
 * @return u8 0--解析失败 1--解析成功
 */
u8 Check_Package_Crc16(u8 *get_data, u16 data_len)
{
	return Verify_CRC16_Check_Sum(get_data, data_len);
}


/**
 * @brief 解析命令ID数据
 * 
 * @param get_data 需要解析的数据包
 * @return uint16_t 命令ID cmd_id
 */
uint16_t Analysis_Cmd_Id(u8 *get_data)
{
	return get_data[5] + (get_data[6]<<8);
}


/**
 * @brief 解析data数据
 * 
 * @param get_data 需要解析的数据包
 * @param data_len data的长度
 * @return uint8_t 0--解析失败 1--解析成功
 */
uint8_t Analysis_Data(u8 *get_data, uint16_t data_len)
{
	Judge_System_Connect_Item_Node* node = &judge_system_connect_root;
	uint16_t cmd_id = get_data[5] | (get_data[6]<<8);
	
	// 遍历链表所有节点
	while( node->next != NULL )
	{
		node = node->next;
		if(cmd_id == node->data.cmd_id)
		{
			if (data_len == node->data.data_len)
			{
				memcpy(node->data.save_space, &get_data[7], data_len);
				if(node->data.Callback != NULL)
					node->data.Callback(cmd_id);
				return 1u;
			}
			else
			{
				return 0u;
			}
		}
	}
	return 0x0A;
}

// 不该放这的函数，之后必须改掉
// 判断1号17mm发射机构是否超热量
u8 Is_Id1_17mm_Excess_Heat(const Judge_data_t* judge_data)
{
	if(judge_data->ext_game_robot_status_t.shooter_id1_17mm_cooling_limit == 65535 || Get_Module_Online_State(5) == 0)
	{
		return 0;
	}
	if(judge_data->ext_game_robot_status_t.shooter_id1_17mm_cooling_limit <= (judge_data->power_heat_data.shooter_id1_17mm_cooling_heat + 12 ))
	{
		return 1;
	}
	return 0;
}


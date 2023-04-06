#include "judge_system.h"
#include "string.h"
#include "math2.h"
#include "crc_check.h"
#include "remoter_task.h"
#include "shooter_task.h"
#include "power_output.h"
#include "detect_task.h"

/* ����ϵͳ���Ժ궨�� */
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

//��������
void Find_All_A5(u8 *get_data, u16 data_len, u8 *r_position, u8 *r_a5_length);
uint8_t Analysis_Frame_Header(u8 *get_data, u16 *r_data_length, u8 *r_seq);
u8 Check_Package_Crc16(u8 *get_data, u16 data_len);
uint16_t Analysis_Cmd_Id(u8 *get_data);
uint8_t Analysis_Data(u8 *get_data, uint16_t data_len);

//����ϵͳ���ݶ���
static Judge_data_t judge_data;

/**
 * @brief ��ȡ����ϵͳ����
 * 
 * @return const Judge_data_t* ���ص�����ͷָ��
 */
const Judge_data_t* Get_Judge_Data(void)
{
	return &judge_data;
}


// ����ϵͳ���������б�����ͷ�ڵ�
static Judge_System_Connect_Item_Node judge_system_connect_root;

/**
 * @brief ע�ᵥ������������Ŀ����ʹ�ý�������֮ǰ�������ע�ᣩ
 * 
 * @param root ͷ�ڵ�ָ��
 * @param new_item ����Ŀָ�룬������Ϊ�ֲ�������ȫ�ֱ�������ڴ��������Ŀռ������
 * @param cmd_id ��Ӧ������ID
 * @param data_len ��Ӧ������֡����
 * @param save_space ��Ӧ�����ݴ�ſռ䣨ǿ�ҽ���ʹ�ùٷ�����ϵͳ�û��ֲ����ṩ�Ľṹ�壩
 */
void _Judge_System_Connect_Register(Judge_System_Connect_Item_Node *root, Judge_System_Connect_Item_Node *new_item, const uint16_t cmd_id, const uint16_t data_len, void *save_space, void(*Callback)(unsigned short))
{
	// ͷ�ڵ�
	Judge_System_Connect_Item_Node *node = root;

	// �������ֵ
	new_item->data.cmd_id = cmd_id;
	new_item->data.data_len = data_len;
	new_item->data.save_space = save_space;
	new_item->data.Callback = Callback;
	new_item->next = NULL;

	// ����Ŀ��������β��
	while( node->next != NULL )
	{
		node = node->next;
	}
	node->next = new_item;
}

/**
 * @brief ��ʼ������ϵͳ���������б���Ҫ���������ݶ�����ó�ʼ������
 * 
 */
void Judge_System_Connect_List_Init(void)
{
	// ͷ�ڵ��ʼ��
	judge_system_connect_root.data.cmd_id = 0;
	judge_system_connect_root.data.data_len = 0;
	judge_system_connect_root.data.save_space = NULL;
	judge_system_connect_root.data.Callback = NULL;
	judge_system_connect_root.next = NULL;
 
	// ��ӽ����ڵ�
	Judge_System_Connect_Item_Register(0x0102, 4, (void*)&judge_data.ext_supply_projectile_action_t, NULL);
	Judge_System_Connect_Item_Register(0x0201, 27, (void*)&judge_data.ext_game_robot_status_t, NULL);
	Judge_System_Connect_Item_Register(0x0202, 16, (void*)&judge_data.ext_power_heat_data_t, NULL);
	Judge_System_Connect_Item_Register(0x0204, 1, (void*)&judge_data.ext_buff_t, NULL);
	Judge_System_Connect_Item_Register(0x0207, 7, (void*)&judge_data.ext_shoot_data_t, NULL);
}


/**
 * @brief ��������ϵͳ����
 * 
 * @param get_data ��Ҫ������֡ͷ����
 * @param data_len ���ݳ���
 * @return uint8_t 0--����ʧ�� 1--�����ɹ�
 */
uint8_t Analysis_Judge_System(u8 *get_data, u16 data_len)
{
	u8 a5_position[8]; //0xA5��λ��
	u8 a5_number = 0;  //0xA5�ĸ��������ݰ�������
	u16 data_length[8];  //ÿ��data���ݰ��ĳ���
	
	//Ѱ��֡ͷ
	Find_All_A5(get_data, data_len, a5_position, &a5_number);
	
	//�����������ݰ�
	for(u8 i=0; i<a5_number; i++)
	{
		//����֡ͷ����
		if( Analysis_Frame_Header(&get_data[ (a5_position[i]) ], &data_length[i], NULL) == 0)
		{
			JUDGE_LOG("Analysis No.%d frame header error.", i);
			continue;
		}
		
		//����CRC16У��
		if( Check_Package_Crc16(&get_data[ (a5_position[i]) ], (data_length[i]+9)) == 0 )
		{
			JUDGE_LOG("CRC16 check No.%d fail.", i);
			continue;
		}
		
		//���������ݰ�
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
 * @brief �ҵ�����֡ͷ��ͷ
 * 
 * @param get_data ��Ҫ������֡ͷ����
 * @param data_len ���ݳ���
 * @param r_position ���0xA5λ������
 * @param r_a5_length �������鳤�ȵĳ���
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
 * @brief ����֡ͷ����
 * 
 * @param get_data ��Ҫ���������ݰ�
 * @param r_data_length ��������֡�� data �ĳ���
 * @param r_seq ����seqֵ
 * @return uint8_t 0--����ʧ�� 1--�����ɹ�
 */
uint8_t Analysis_Frame_Header(u8 *get_data, u16 *r_data_length, u8 *r_seq)
{
	// JUDGE_ARRAY("head", get_data, 5);
	
	//�ж�֡ͷ��ͷ�Ƿ�Ϊ0xA5
	if(get_data[0] != 0xA5)
	{
		JUDGE_ERROR(501);
		return 0;
	}
	
	//֡ͷCRC8У��
	if( Verify_CRC8_Check_Sum(get_data, 5) == 0)
	{
		JUDGE_ERROR(502);
		return 0;
	}
	
	//����data�ĳ���
	if(r_data_length != NULL)
	{
		*r_data_length = get_data[1] | (get_data[2] << 8);
	}
	
	//����seq
	if(r_seq != NULL)
	{
		*r_seq = get_data[3];
	}
	
	return 1;
}


/**
 * @brief һ�����ݰ�����CRC16У��
 * 
 * @param get_data ��Ҫ���������ݰ�
 * @param data_len ������
 * @return u8 0--����ʧ�� 1--�����ɹ�
 */
u8 Check_Package_Crc16(u8 *get_data, u16 data_len)
{
	return Verify_CRC16_Check_Sum(get_data, data_len);
}


/**
 * @brief ��������ID����
 * 
 * @param get_data ��Ҫ���������ݰ�
 * @return uint16_t ����ID cmd_id
 */
uint16_t Analysis_Cmd_Id(u8 *get_data)
{
	return get_data[5] + (get_data[6]<<8);
}


/**
 * @brief ����data����
 * 
 * @param get_data ��Ҫ���������ݰ�
 * @param data_len data�ĳ���
 * @return uint8_t 0--����ʧ�� 1--�����ɹ�
 */
uint8_t Analysis_Data(u8 *get_data, uint16_t data_len)
{
	Judge_System_Connect_Item_Node* node = &judge_system_connect_root;
	uint16_t cmd_id = get_data[5] | (get_data[6]<<8);
	
	// �����������нڵ�
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

// ���÷���ĺ�����֮�����ĵ�
// �ж�1��17mm��������Ƿ�����
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


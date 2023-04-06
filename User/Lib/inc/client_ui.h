#ifndef __CLIENT_UI
#define __CLIENT_UI

#include "stm32f4xx.h"
#include "stdarg.h"

// ��1�ֽڶ���
#pragma pack(1)

// �ͻ���UI���֡����
#define UI_MAX_SEND_FRAME_BYTE 113u

// �����˽�ɫ�Զ�Ӧ�ͻ�������
#define Robot_ID current_robot_id
#define Cilent_ID current_cilent_id

// ��ʼ��־
#define UI_SOF 0xA5

// CMD_ID����(�����˼佻�����ݣ����ͷ��������ͣ����� 10Hz)
#define UI_CMD_Robo_Exchange 0x0301

// ����ID����
#define UI_Data_ID_Del 0x100
#define UI_Data_ID_Draw1 0x101
#define UI_Data_ID_Draw2 0x102
#define UI_Data_ID_Draw5 0x103
#define UI_Data_ID_Draw7 0x104
#define UI_Data_ID_DrawChar 0x110

// �췽������ID
#define UI_Data_RobotID_RHero 1
#define UI_Data_RobotID_REngineer 2
#define UI_Data_RobotID_RStandard1 3
#define UI_Data_RobotID_RStandard2 4
#define UI_Data_RobotID_RStandard3 5
#define UI_Data_RobotID_RAerial 6
#define UI_Data_RobotID_RSentry 7
#define UI_Data_RobotID_RRadar 9

// ����������ID
#define UI_Data_RobotID_BHero 101
#define UI_Data_RobotID_BEngineer 102
#define UI_Data_RobotID_BStandard1 103
#define UI_Data_RobotID_BStandard2 104
#define UI_Data_RobotID_BStandard3 105
#define UI_Data_RobotID_BAerial 106
#define UI_Data_RobotID_BSentry 107
#define UI_Data_RobotID_BRadar 109

// �췽������ID
#define UI_Data_CilentID_RHero 0x0101
#define UI_Data_CilentID_REngineer 0x0102
#define UI_Data_CilentID_RStandard1 0x0103
#define UI_Data_CilentID_RStandard2 0x0104
#define UI_Data_CilentID_RStandard3 0x0105
#define UI_Data_CilentID_RAerial 0x0106

// ����������ID
#define UI_Data_CilentID_BHero 0x0165
#define UI_Data_CilentID_BEngineer 0x0166
#define UI_Data_CilentID_BStandard1 0x0167
#define UI_Data_CilentID_BStandard2 0x0168
#define UI_Data_CilentID_BStandard3 0x0169
#define UI_Data_CilentID_BAerial 0x016A

// ɾ������
#define UI_Data_Del_NoOperate 0
#define UI_Data_Del_Layer 1
#define UI_Data_Del_ALL 2

// ͼ�����ò���__ͼ�β���
#define UI_Graph_ADD 1
#define UI_Graph_Change 2
#define UI_Graph_Del 3

// ͼ�����ò���__ͼ������
#define UI_Graph_Line 0		 //ֱ��
#define UI_Graph_Rectangle 1 //����
#define UI_Graph_Circle 2	 //��Բ
#define UI_Graph_Ellipse 3	 //��Բ
#define UI_Graph_Arc 4		 //Բ��
#define UI_Graph_Float 5	 //������
#define UI_Graph_Int 6		 //����
#define UI_Graph_Char 7		 //�ַ���

// ͼ�����ò���__ͼ����ɫ
#define UI_Color_Main 0 //������ɫ
#define UI_Color_Yellow 1
#define UI_Color_Green 2
#define UI_Color_Orange 3
#define UI_Color_Purplish_red 4 //�Ϻ�ɫ
#define UI_Color_Pink 5
#define UI_Color_Cyan 6 //��ɫ
#define UI_Color_Black 7
#define UI_Color_White 8

// ����ʽ
// frame_header (5-byte) cmd_id (2-byte) data (n-byte) frame_tail (2-byte��CRC16������У��)

typedef struct
{
	uint8_t SOF;		  //��ʼ�ֽ�,�̶�0xA5
	uint16_t Data_Length; //֡���ݳ���
	uint8_t Seq;		  //�����
	uint8_t CRC8;		  // CRC8У��ֵ
	uint16_t CMD_ID;	  //����ID
} UI_Packhead;			  //֡ͷ

typedef struct
{
	uint16_t Data_ID;	  //����ID
	uint16_t Sender_ID;	  //������ID
	uint16_t Receiver_ID; //������ID
} UI_Data_Operate;		  //��������֡

typedef struct
{
	uint8_t Delete_Operate; //ɾ������
	uint8_t Layer;			//ɾ��ͼ��
} UI_Data_Delete;			//ɾ��ͼ��֡

typedef struct
{
	uint8_t graphic_name[3];
	uint32_t operate_tpye : 3;
	uint32_t graphic_tpye : 3;
	uint32_t layer : 4;
	uint32_t color : 4;
	uint32_t start_angle : 9;
	uint32_t end_angle : 9;
	uint32_t width : 10;
	uint32_t start_x : 11;
	uint32_t start_y : 11;
	uint32_t radius : 10;
	uint32_t end_x : 11;
	uint32_t end_y : 11; //ͼ������
} Graph_Data;

typedef struct
{
	Graph_Data Graph_Control;
	uint8_t show_Data[30]; // UI��ӡ���ַ�������
} String_Data;


uint8_t UI_Set_Comparable_Id(const uint16_t robot_id);
uint16_t UI_Get_Send_Data_Len(const void* point);

void UI_Delete(uint8_t* send_buf, uint8_t Del_Operate,uint8_t Del_Layer);

void Line_Draw(Graph_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Graph_Width, u32 Start_x, u32 Start_y, u32 End_x, u32 End_y);
void Rectangle_Draw(Graph_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Graph_Width, u32 Start_x, u32 Start_y, u32 End_x, u32 End_y);
void Arc_Draw(Graph_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Graph_StartAngle, u32 Graph_EndAngle, u32 Graph_Width, u32 Start_x, u32 Start_y, u32 x_Length, u32 y_Length);
void Float_Draw(Graph_Data *image, char imagename[3], uint32_t Graph_Operate, uint32_t Graph_Layer, uint32_t Graph_Color, uint32_t Graph_Size, uint32_t Graph_Digit, uint32_t Graph_Width, uint32_t Start_x, uint32_t Start_y, float Graph_Float);
int UI_ReFresh(uint8_t *send_buf, int cnt, ...);

void Char_Draw(String_Data *image, char imagename[3], u32 Graph_Operate, u32 Graph_Layer, u32 Graph_Color, u32 Graph_Size, u32 Graph_Digit, u32 Graph_Width, u32 Start_x, u32 Start_y, char *Char_Data);
int Char_ReFresh(uint8_t *send_buf, String_Data string_Data);


#endif

#include "client_ui.h"
#include "string.h"

// UI专用的CRC校验函数
unsigned char Get_CRC8_Check_Sum_UI(unsigned char *pchMessage, unsigned int dwLength, unsigned char ucCRC8);
uint16_t Get_CRC16_Check_Sum_UI(uint8_t *pchMessage, uint32_t dwLength, uint16_t wCRC);

// 包序号
static unsigned char UI_Seq = 0;
static uint16_t current_robot_id = 0;
static uint16_t current_cilent_id =0;

/**
 * @brief 获取机器人对应的客户端ID
 * 
 * @param robot_id 机器人ID
 * @return uint16_t 对应的客户端ID
 */
uint16_t UI_Get_Comparable_Client_Id(const uint16_t robot_id)
{
	return robot_id+0x100;
}

/**
 * @brief 设置当前机器人ID，客户端会同时设置成对应的ID
 * 
 * @param robot_id 机器人ID
 * @return uint8_t 成功1，失败0
 */
uint8_t UI_Set_Comparable_Id(const uint16_t robot_id)
{
	if (!((robot_id >= UI_Data_RobotID_RHero && robot_id <= UI_Data_RobotID_RRadar) 
	|| (robot_id >= UI_Data_RobotID_BHero && robot_id <= UI_Data_RobotID_BRadar)))
	{
		return 0u;
	}
	current_robot_id = robot_id;
	current_cilent_id = UI_Get_Comparable_Client_Id(robot_id);
	return 1u;
}

/**
 * @brief 获取UI要发送的数据帧的总长度，数据帧必须是无错误的
 * 
 * @param point 数据帧的头指针
 * @return uint16_t 数据总长度
 */
uint16_t UI_Get_Send_Data_Len(const void* point)
{
	return ((UI_Packhead*)point)->Data_Length + 9u;
}

/**
 * @brief 删除UI图层
 * 
 * @param send_buf 发送数据存放区（需、建议大于113Byte）
 * @param Del_Operate 操作方式
 * @param Del_Layer 操作图层
 */
void UI_Delete(uint8_t* send_buf, uint8_t Del_Operate,uint8_t Del_Layer)
{
	unsigned char *framepoint = 0; // 读写指针
	unsigned char *sendpoint = 0;  // 发送数据指针
	uint16_t frametail = 0xFFFF;   // CRC16校验值

	UI_Packhead framehead;	  // 帧头
	UI_Data_Operate datahead; // 操作包
	UI_Data_Delete del;		  // 删除数据的信息

	framepoint = (unsigned char *)&framehead;
	sendpoint = (unsigned char *)send_buf;

	framehead.SOF = UI_SOF;
	framehead.Data_Length = 8;
	framehead.Seq = UI_Seq;
	framehead.CRC8 = Get_CRC8_Check_Sum_UI(framepoint, 4, 0xFF);
	framehead.CMD_ID = UI_CMD_Robo_Exchange;

	// 填充操作数据
	datahead.Data_ID = UI_Data_ID_Del;
	datahead.Sender_ID = Robot_ID;
	datahead.Receiver_ID = Cilent_ID;

	// 控制信息
	del.Delete_Operate = Del_Operate;
	del.Layer = Del_Layer;

	// CRC16校验值计算
	frametail = Get_CRC16_Check_Sum_UI(framepoint, sizeof(framehead), frametail);
	framepoint = (unsigned char *)&datahead;
	frametail = Get_CRC16_Check_Sum_UI(framepoint, sizeof(datahead), frametail);
	framepoint = (unsigned char *)&del;
	frametail = Get_CRC16_Check_Sum_UI(framepoint, sizeof(del), frametail);

	// 拷贝帧头
	framepoint=(unsigned char *)&framehead;
	memcpy(sendpoint, framepoint, sizeof(framehead));
	sendpoint += sizeof(framehead);

	// 数据段头
	framepoint=(unsigned char *)&datahead;
	memcpy(sendpoint, framepoint, sizeof(datahead));
	sendpoint += sizeof(datahead);

	// 数据段控制部分
	framepoint=(unsigned char *)&del;
	memcpy(sendpoint, framepoint, sizeof(del));
	sendpoint += sizeof(del);

	// 帧尾，CRC16校验值
	framepoint=(unsigned char *)&frametail;
	memcpy(sendpoint, framepoint, sizeof(frametail));
	sendpoint += sizeof(frametail);
	
	UI_Seq++; //包序号+1
}


/**
 * @brief 绘制直线
 * 
 * @param image 类型变量指针，用于存放图形数据
 * @param imagename 图片名称，用于标识更改
 * @param Graph_Operate 图片操作，见头文件
 * @param Graph_Layer 图层0-9
 * @param Graph_Color 图形颜色
 * @param Graph_Width 图形线宽
 * @param Start_x 开始坐标
 * @param Start_y 开始坐标
 * @param End_x 结束坐标
 * @param End_y 结束坐标
 */
void Line_Draw(Graph_Data *image, char imagename[3], uint32_t Graph_Operate, uint32_t Graph_Layer, uint32_t Graph_Color, uint32_t Graph_Width, uint32_t Start_x, uint32_t Start_y, uint32_t End_x, uint32_t End_y)
{
	for (int i = 0; i < 3 && imagename[i] != '\0'; i++)
	{
		image->graphic_name[2 - i] = imagename[i];
	}
	image->operate_tpye = Graph_Operate;
	image->layer = Graph_Layer;
	image->color = Graph_Color;
	image->width = Graph_Width;
	image->start_x = Start_x;
	image->start_y = Start_y;
	image->end_x = End_x;
	image->end_y = End_y;
}


/**
 * @brief 绘制矩形
 * 
 * @param image 类型变量指针，用于存放图形数据
 * @param imagename 图片名称，用于标识更改
 * @param Graph_Operate 图片操作，见头文件
 * @param Graph_Layer 图层0-9
 * @param Graph_Color 图形颜色
 * @param Graph_Width 图形线宽
 * @param Start_x 开始坐标
 * @param Start_y 开始坐标
 * @param End_x 结束坐标（对顶角坐标）
 * @param End_y 结束坐标（对顶角坐标）
 */
void Rectangle_Draw(Graph_Data *image, char imagename[3], uint32_t Graph_Operate, uint32_t Graph_Layer, uint32_t Graph_Color, uint32_t Graph_Width, uint32_t Start_x, uint32_t Start_y, uint32_t End_x, uint32_t End_y)
{
	for (int i = 0; i < 3 && imagename[i] != '\0'; i++)
	{
		image->graphic_name[2 - i] = imagename[i];
	}
	image->graphic_tpye = UI_Graph_Rectangle;
	image->operate_tpye = Graph_Operate;
	image->layer = Graph_Layer;
	image->color = Graph_Color;
	image->width = Graph_Width;
	image->start_x = Start_x;
	image->start_y = Start_y;
	image->end_x = End_x;
	image->end_y = End_y;
}


/**
 * @brief 绘制整圆
 * 
 * @param image 类型变量指针，用于存放图形数据
 * @param imagename 图片名称，用于标识更改
 * @param Graph_Operate 图片操作，见头文件
 * @param Graph_Layer 图层0-9
 * @param Graph_Color 图形颜色
 * @param Graph_Width 图形线宽
 * @param Start_x 圆心坐标
 * @param Start_y 圆心坐标
 * @param Graph_Radius 图形半径
 */
void Circle_Draw(Graph_Data *image, char imagename[3], uint32_t Graph_Operate, uint32_t Graph_Layer, uint32_t Graph_Color, uint32_t Graph_Width, uint32_t Start_x, uint32_t Start_y, uint32_t Graph_Radius)
{
	for (int i = 0; i < 3 && imagename[i] != '\0'; i++)
	{
		image->graphic_name[2 - i] = imagename[i];
	}
	image->graphic_tpye = UI_Graph_Circle;
	image->operate_tpye = Graph_Operate;
	image->layer = Graph_Layer;
	image->color = Graph_Color;
	image->width = Graph_Width;
	image->start_x = Start_x;
	image->start_y = Start_y;
	image->radius = Graph_Radius;
}

/**
 * @brief 绘制圆弧
 * 
 * @param image 类型变量指针，用于存放图形数据
 * @param imagename 图片名称，用于标识更改
 * @param Graph_Operate 图片操作，见头文件
 * @param Graph_Layer 图层0-9
 * @param Graph_Color 图形颜色
 * @param Graph_StartAngle 开始角度
 * @param Graph_EndAngle 终止角度
 * @param Graph_Width 图形线宽
 * @param Start_x 圆心坐标
 * @param Start_y 圆心坐标
 * @param x_Length x,y方向上轴长，参考椭圆
 * @param y_Length x,y方向上轴长，参考椭圆
 */
void Arc_Draw(Graph_Data *image, char imagename[3], uint32_t Graph_Operate, uint32_t Graph_Layer, uint32_t Graph_Color, uint32_t Graph_StartAngle, uint32_t Graph_EndAngle, uint32_t Graph_Width, uint32_t Start_x, uint32_t Start_y, uint32_t x_Length, uint32_t y_Length)
{
	for (int i = 0; i < 3 && imagename[i] !='\0'; i++)
	{
		image->graphic_name[2 - i] = imagename[i];
	}
	image->graphic_tpye = UI_Graph_Arc;
	image->operate_tpye = Graph_Operate;
	image->layer = Graph_Layer;
	image->color = Graph_Color;
	image->width = Graph_Width;
	image->start_x = Start_x;
	image->start_y = Start_y;
	image->start_angle = Graph_StartAngle;
	image->end_angle = Graph_EndAngle;
	image->end_x = x_Length;
	image->end_y = y_Length;
}

/**
 * @brief 绘制浮点型数据
 * 
 * @param image 类型变量指针，用于存放图形数据
 * @param imagename 图片名称，用于标识更改
 * @param Graph_Operate 图片操作，见头文件
 * @param Graph_Layer 图层0-9
 * @param Graph_Color 图形颜色
 * @param Graph_Size 字号
 * @param Graph_Digit 小数位数
 * @param Graph_Width 图形线宽
 * @param Start_x 开始坐标
 * @param Start_y 开始坐标
 * @param Graph_Float 要显示的变量
 */
void Float_Draw(Graph_Data *image, char imagename[3], uint32_t Graph_Operate, uint32_t Graph_Layer, uint32_t Graph_Color, uint32_t Graph_Size, uint32_t Graph_Digit, uint32_t Graph_Width, uint32_t Start_x, uint32_t Start_y, float Graph_Float)
{
	for (int i = 0; i < 3 && imagename[i] != '\0'; i++)
	{
		image->graphic_name[2 - i] = imagename[i];
	}
	image->graphic_tpye = UI_Graph_Float;
	image->operate_tpye = Graph_Operate;
	image->layer = Graph_Layer;
	image->color = Graph_Color;
	image->width = Graph_Width;
	image->start_x = Start_x;
	image->start_y = Start_y;
	image->start_angle = Graph_Size;
	image->end_angle = Graph_Digit;
	int32_t test = Graph_Float * 1000;
	(*(int32_t *)(((int8_t *)image) + 11u)) = test;
}


/**
 * @brief 绘制字符型数据
 * 
 * @param image Graph_Data类型变量指针，用于存放图形数据
 * @param imagename 图片名称，用于标识更改
 * @param Graph_Operate 图片操作，见头文件
 * @param Graph_Layer 图层0-9
 * @param Graph_Color 图形颜色
 * @param Graph_Size 字号
 * @param Graph_Digit 字符个数
 * @param Graph_Width 图形线宽
 * @param Start_x 开始坐标
 * @param Start_y 开始坐标
 * @param Char_Data 待发送字符串开始地址
 */
void Char_Draw(String_Data *image, char imagename[3], uint32_t Graph_Operate, uint32_t Graph_Layer, uint32_t Graph_Color, uint32_t Graph_Size, uint32_t Graph_Digit, uint32_t Graph_Width, uint32_t Start_x, uint32_t Start_y, char *Char_Data)
{
	int i;
	for (int i = 0; i < 3 && imagename[i] != '\0'; i++)
	{
		image->Graph_Control.graphic_name[2 - i] = imagename[i];
	}
	image->Graph_Control.graphic_tpye = UI_Graph_Char;
	image->Graph_Control.operate_tpye = Graph_Operate;
	image->Graph_Control.layer = Graph_Layer;
	image->Graph_Control.color = Graph_Color;
	image->Graph_Control.width = Graph_Width;
	image->Graph_Control.start_x = Start_x;
	image->Graph_Control.start_y = Start_y;
	image->Graph_Control.start_angle = Graph_Size;
	image->Graph_Control.end_angle = Graph_Digit;
	for (i = 0; i < Graph_Digit; i++)
	{
		image->show_Data[i] = *Char_Data;
		Char_Data++;
	}
}


/**
 * @brief UI推送函数（使更改生效），该函数只能推送1，2，5，7个图形，其他数目协议未涉及
 * 
 * @param send_buf 
 * @param cnt 图形个数
 * @param ... Graph_Data类型数据，图形变量参数
 * @return int 成功推送0，推送失败other
 */
int UI_ReFresh(uint8_t *send_buf, int cnt, ...)
{
	int i;
	unsigned char *framepoint;	 //读写指针
	unsigned char *sendpoint;	 //发送数组指针
	uint16_t frametail = 0xFFFF; // CRC16校验值

	Graph_Data imageData;
	UI_Packhead framehead;
	UI_Data_Operate datahead;

	va_list ap;
	va_start(ap, cnt);

	framepoint = (unsigned char *)&framehead;
	sendpoint = (unsigned char *)send_buf;

	framehead.SOF = UI_SOF;
	framehead.Data_Length = 6 + cnt * 15;
	framehead.Seq = UI_Seq;
	framehead.CRC8 = Get_CRC8_Check_Sum_UI(framepoint, 4, 0xFF);
	framehead.CMD_ID = UI_CMD_Robo_Exchange; //填充包头数据

	switch (cnt)
	{
	case 1:
		datahead.Data_ID = UI_Data_ID_Draw1;
		break;
	case 2:
		datahead.Data_ID = UI_Data_ID_Draw2;
		break;
	case 5:
		datahead.Data_ID = UI_Data_ID_Draw5;
		break;
	case 7:
		datahead.Data_ID = UI_Data_ID_Draw7;
		break;
	default:
		return (-1);
	}

	// 填充操作数据
	datahead.Sender_ID = Robot_ID;
	datahead.Receiver_ID = Cilent_ID;

	// CRC16校验值计算（部分）
	framepoint = (unsigned char *)&framehead;
	frametail = Get_CRC16_Check_Sum_UI(framepoint, sizeof(framehead), frametail);
	framepoint = (unsigned char *)&datahead;
	frametail = Get_CRC16_Check_Sum_UI(framepoint, sizeof(datahead), frametail);

	// 帧头部分
	framepoint = (unsigned char *)&framehead;
	memcpy(sendpoint, framepoint, sizeof(framehead));
	sendpoint += sizeof(framehead);

	// 数据段头部
	framepoint = (unsigned char *)&datahead;
	memcpy(sendpoint, framepoint, sizeof(datahead));
	sendpoint += sizeof(datahead);

	for (i = 0; i < cnt; i++)
	{
		imageData = va_arg(ap, Graph_Data);

		framepoint = (unsigned char *)&imageData;

		// 计算CRC16校验（部分）
		frametail = Get_CRC16_Check_Sum_UI(framepoint, sizeof(imageData), frametail);

		// 拷贝图片帧
		memcpy(sendpoint, framepoint, sizeof(imageData));
		sendpoint += sizeof(imageData);
	}

	framepoint = (unsigned char *)&frametail;
	memcpy(sendpoint, framepoint, sizeof(frametail));
	sendpoint += sizeof(frametail);

	va_end(ap);

	// 包序号+1
	UI_Seq++;
	return 0;
}


/**
 * @brief UI推送字符（使更改生效），最大30个字符
 * 
 * @param send_buf UI待发送的数据缓冲区
 * @param string_Data 字符串变量参数
 * @return int 成功推送0，推送失败other
 */
int Char_ReFresh(uint8_t *send_buf, String_Data string_Data)
{
	String_Data imageData;
	unsigned char *framepoint;	 //读写指针
	unsigned char *sendpoint;	 //发送指针
	uint16_t frametail = 0xFFFF; // CRC16校验值

	UI_Packhead framehead;
	UI_Data_Operate datahead;
	imageData=string_Data;
	
	framepoint=(unsigned char *)&framehead;
	sendpoint = (unsigned char *)send_buf;

	//填充包头数据
	framehead.SOF = UI_SOF;
	framehead.Data_Length = 6 + 45;
	framehead.Seq = UI_Seq;
	framehead.CRC8 = Get_CRC8_Check_Sum_UI(framepoint, 4, 0xFF);
	framehead.CMD_ID = UI_CMD_Robo_Exchange;

	//填充操作数据
	datahead.Data_ID = UI_Data_ID_DrawChar;
	datahead.Sender_ID = Robot_ID;
	datahead.Receiver_ID = Cilent_ID;

	//CRC16校验值计算
	framepoint=(unsigned char *)&framehead;
	frametail=Get_CRC16_Check_Sum_UI(framepoint,sizeof(framehead),frametail);
	framepoint=(unsigned char *)&datahead;
	frametail=Get_CRC16_Check_Sum_UI(framepoint,sizeof(datahead),frametail);
	framepoint=(unsigned char *)&imageData;
	frametail=Get_CRC16_Check_Sum_UI(framepoint,sizeof(imageData),frametail);
	
	framepoint=(unsigned char *)&framehead;
	memcpy(sendpoint, framepoint, sizeof(framehead));
	sendpoint += sizeof(framehead);

	// 发送操作数据
	framepoint = (unsigned char *)&datahead;
	memcpy(sendpoint, framepoint, sizeof(datahead));
	sendpoint += sizeof(datahead);

	// 发送图片帧
	framepoint=(unsigned char *)&imageData;
	memcpy(sendpoint, framepoint, sizeof(imageData));
	sendpoint += sizeof(imageData);
	
	// 发送CRC16校验值
	framepoint=(unsigned char *)&frametail;
	memcpy(sendpoint, framepoint, sizeof(frametail));
	sendpoint += sizeof(frametail);
	
	UI_Seq++; // 包序号+1
	return 0;
}


/* --- BEGIN 客户端UI专用的CRC8、CRC16校验值计算 BEGIN --- */
#ifndef NULL
#define NULL 0
#endif
const unsigned char CRC8_INIT_UI = 0xff; 
const unsigned char CRC8_TAB_UI[256] = 
{ 
	0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41, 
	0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc, 
	0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62, 
	0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff, 
	0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07, 
	0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a, 
	0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24, 
	0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9, 
	0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd, 
	0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50, 
	0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee, 
	0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73, 
	0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b, 
	0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16, 
	0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8, 
	0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35, 
};

/* 
** Descriptions: CRC8 checksum function 
** Input: Data to check,Stream length, initialized checksum
** Output: CRC checksum 
*/
unsigned char Get_CRC8_Check_Sum_UI(unsigned char *pchMessage, unsigned int dwLength, unsigned char ucCRC8)
{
	unsigned char ucIndex;
	while (dwLength--)
	{
		ucIndex = ucCRC8 ^ (*pchMessage++);
		ucCRC8 = CRC8_TAB_UI[ucIndex];
	}
	return (ucCRC8);
}

uint16_t CRC_INIT_UI = 0xffff;
const uint16_t wCRC_Table_UI[256] =
	{
		0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
		0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
		0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
		0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
		0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
		0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
		0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
		0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
		0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
		0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
		0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
		0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
		0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
		0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
		0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
		0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
		0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
		0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
		0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
		0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
		0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
		0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
		0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
		0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
		0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
		0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
		0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
		0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
		0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
		0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
		0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
		0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78};

/* 
** Descriptions: CRC16 checksum function 
** Input: Data to check,Stream length, initialized checksum 
** Output: CRC checksum 
*/
uint16_t Get_CRC16_Check_Sum_UI(uint8_t *pchMessage, uint32_t dwLength, uint16_t wCRC)
{
	uint8_t chData;
	if (pchMessage == NULL)
	{
		return 0xFFFF;
	}
	while (dwLength--)
	{
		chData = *pchMessage++;
		(wCRC) = ((uint16_t)(wCRC) >> 8) ^ wCRC_Table_UI[((uint16_t)(wCRC) ^ (uint16_t)(chData)) & 0x00ff];
	}
	return wCRC;
}

/* --- BEGIN 客户端UI专用的CRC8、CRC16校验值计算 BEGIN --- */

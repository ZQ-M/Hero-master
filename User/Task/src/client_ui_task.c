#include "client_ui_task.h"

void StartClientUiTask(void const *argument)
{
    osDelay(1000);
    Uart8_Dma_Tx_Init();
    uint8_t data[4] = {0x01, 0x02, 0x03, 0x04};
    for (;;)
    {
        // printf_ui("%d %d %d %d\r\n", 1, 2, 3, 4);
        Uart8_Transmit_Dma((uint32_t)data, 4);
        osDelay(50);
    }
}


/*
 * @brief HLL战队-英雄操作手UI
 * @note  初步计划加入静态UI:榴弹提示线，
 *                   动态UI:摩擦轮转速，上一发枪口速度,超级电容
 * 
*/
/* 数据帧发送上限113 */
uint8_t SendBuff[113];

/* 调用UI结构体--榴弹提示线上部分 */
Graph_Data line_1, line_2, line_3, line_4, line_5, line_6,Middle_Point;
/* 调用UI结构体--榴弹提示线下部分 */
Graph_Data line_Mid1, line_Mid2, line_A1, line_A2, line_B1, line_B2,line_C1;
/* 调用UI结构体--上一发枪口速度，超级电容 */
Graph_Data MuzzleSpeed SuperCapacitors;

/* 获得摩擦轮数据 *//*用shoottask里的Parse_Friction_Wave_Motor_Feedback_Data，还是有其他的接口*/
static const uint8_t friction_motor_num = 2;
static Motor_Measure_t friction_motor_feedback_data[friction_motor_num];
/* 获得裁判系统数据 */
const static Judge_data_t *UI_JUDGE_DATA;
/* 获取遥控器数据 */
const static Rc_Ctrl_t *UI_RC_DATA;

/* 判断是红方还是蓝方，决定发送ID */
#define HERO_IN_WHICH_Camp  UI_JUDGE_DATA->ext_game_robot_status_t.robot_id==0x0001?UI_Data_CilentID_BHero:UI_Data_CilentID_RHero

void OperatorUItask(void const *argument)
{
  UI_RC_DATA    = Get_Rc_Parsed_RemoteData_Pointer();
  UI_JUDGE_DATA = Get_Judge_Data();
  for(;;)
  {
  /* 设置机器人ID */
  UI_Set_Comparable_Id(HERO_IN_WHICH_Camp);
  /* 通过手动按下字母发送静态UI - 榴弹提示线 */
  GrenadeCueLine();
  /* 自动更新动态UI - 榴弹提示线 */
  Super_Capacitance();
  }
}

/*********************************************************************
 * @brief 静态UI - 榴弹提示线
 * 
 * @param Color 
*********************************************************************/
void GrenadeCueLine(uint8_t Color)
{
		/* 榴弹提示线上部分 */
		Line_Draw(&line_1,"001",UI_Graph_ADD,9,Color,2,961,550,961,600);//上
		Line_Draw(&line_2,"002",UI_Graph_ADD,9,Color,2,960,550,960,600);//上偏移
		Line_Draw(&line_3,"003",UI_Graph_ADD,9,Color,2,950,541,900,541);//左
		Line_Draw(&line_4,"004",UI_Graph_ADD,9,Color,2,950,540,900,540);//左偏移
		Line_Draw(&line_5,"005",UI_Graph_ADD,9,Color,2,970,541,1020,541);//右
		Line_Draw(&line_6,"006",UI_Graph_ADD,9,Color,2,970,540,1020,540);//右偏移
		Line_Draw(&Middle_Point,"007",UI_Graph_ADD,9,Color,2,960,540,961,541);//右偏移
    /* 将更新数据写入 */
		UI_ReFresh(SendBuff,7,line_1,line_2,line_3,line_4,line_5,line_6,Middle_Point);
    /* 获得发送长度 */
    uint16_t len1=UI_Get_Send_Data_Len(SendBuff);
    /* 发送 */
    osDelay(100);

		/* 榴弹提示线下部分 */
		Line_Draw(&line_Mid1,"011",UI_Graph_ADD,8,Color,1,961,530,961,330);//中垂线
		Line_Draw(&line_Mid2,"012",UI_Graph_ADD,8,Color,1,960,530,960,330);//中垂线偏移
		Line_Draw(&line_A1  ,"013",UI_Graph_ADD,8,Color,1,860,430,1060,430);//A抛物线
		Line_Draw(&line_A2  ,"014",UI_Graph_ADD,8,Color,1,860,429,1060,429);//A抛物线扶正
		Line_Draw(&line_B1  ,"015",UI_Graph_ADD,8,Color,1,900,380,1020,380);//B抛物线
		Line_Draw(&line_B2  ,"016",UI_Graph_ADD,8,Color,1,900,379,1020,379);//B抛物线扶正
		Line_Draw(&line_C1  ,"017",UI_Graph_ADD,8,Color,1,920,330,1000,330);//C抛物线
		/* 将更新数据写入 */
    UI_ReFresh(SendBuff,7,line_Mid1,line_Mid2,line_A1,line_A2,line_B1,line_B2,line_C1);
    /* 获得发送长度 */
    uint16_t len2=UI_Get_Send_Data_Len(SendBuff);
    /* 发送 */
    osDelay(100);
}


void FrictionWheelSpeed(void)
{

}

void Last_42mm_Speed(void)
{

}

void Super_Capacitance(void)
{
Float_Draw(&SuperCapacitors,"021",UI_Graph_ADD,0,UI_Color_Cyan,12,1,2,800,540,UI_JUDGE_DATA);
UI_ReFresh(SendBuff,1,SuperCapacitors);
/* 发送 */
}

/* 需要重新开始焊接的线材 *///can1，can2Y型线，摩擦轮供电，接收机杜邦线口合并，下载器杜邦线口合并，A板固定
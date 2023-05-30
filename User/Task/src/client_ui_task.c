/*
 * @brief HLL战队-英雄操作手UI
 * @note  初步计划加入静态UI:榴弹提示线，
 *                   动态UI:摩擦轮转速，上一发枪口速度,超级电容
 * 
*/
#include "client_ui_task.h"

/* 数据帧发送上限113 */
static uint8_t SendBuff[113];
/* 设定动态数据的发射间隔 */
const int UI_SEND_DELAY_TIME_MS = 1000;

/* 获得摩擦轮数据 */
static const uint8_t friction_motor_num = 2;
static Motor_Measure_t friction_motor_feedback_data[friction_motor_num];
/* 获得裁判系统数据 */
const static Judge_data_t *UI_JUDGE_DATA;
/* 获取遥控器数据 */
const static Rc_Ctrl_t *UI_RC_DATA;

/* 调用UI结构体--榴弹提示线上部分 */
Graph_Data line_1, line_2, line_3, line_4, line_5, line_6, Middle_Point;
/* 调用UI结构体--榴弹提示线下部分 */
Graph_Data line_Mid1, line_Mid2, line_A1, line_A2, line_B1, line_B2, line_C1;
/* 调用UI结构体--上一发枪口速度，超级电容 */
Graph_Data MuzzleSpeed, SuperCapacitors;

/* 判断是红方还是蓝方，决定发送ID */
#define HERO_IN_WHICH_Camp UI_JUDGE_DATA->ext_game_robot_status_t.robot_id == 0x0001 ? UI_Data_CilentID_BHero : UI_Data_CilentID_RHero

void StartClientUiTask(void const *argument)
{
  osDelay(1000);
  Uart8_Dma_Tx_Init();
  UI_RC_DATA = Get_Rc_Parsed_RemoteData_Pointer();
  UI_JUDGE_DATA = Get_Judge_Data();
  (void)UI_RC_DATA;
  (void)UI_JUDGE_DATA;
  (void)friction_motor_feedback_data;
  for (;;)
  {
    // /* 设置机器人ID */
    // UI_Set_Comparable_Id(HERO_IN_WHICH_Camp);
    // /* 通过手动按下字母发送静态UI - 榴弹提示线 */
    // GrenadeCueLine(UI_Color_Yellow);
    // /* 自动更新动态UI - 榴弹提示线 */
    // Super_Capacitance();

    osDelay(UI_SEND_DELAY_TIME_MS);
  }
}


/*
 * @brief 静态UI - 中国湖榴弹提示线
 * 
 * @param Color UI_Color_Main 
                UI_Color_Yellow
                UI_Color_Green 
                UI_Color_Orange
                UI_Color_Purpli
                UI_Color_Pink 
                UI_Color_Cyan //藏青色
                UI_Color_Black 
                UI_Color_White 
*/
void GrenadeCueLine(uint8_t Color)
{
  /* 榴弹提示线上部分 */
  Line_Draw(&line_1, "001", UI_Graph_ADD, 9, Color, 2, 961, 550, 961, 600);       //上
  Line_Draw(&line_2, "002", UI_Graph_ADD, 9, Color, 2, 960, 550, 960, 600);       //上偏移
  Line_Draw(&line_3, "003", UI_Graph_ADD, 9, Color, 2, 950, 541, 900, 541);       //左
  Line_Draw(&line_4, "004", UI_Graph_ADD, 9, Color, 2, 950, 540, 900, 540);       //左偏移
  Line_Draw(&line_5, "005", UI_Graph_ADD, 9, Color, 2, 970, 541, 1020, 541);      //右
  Line_Draw(&line_6, "006", UI_Graph_ADD, 9, Color, 2, 970, 540, 1020, 540);      //右偏移
  Line_Draw(&Middle_Point, "007", UI_Graph_ADD, 9, Color, 2, 960, 540, 961, 541); //右偏移
  /* 将更新数据写入 */
  UI_ReFresh(SendBuff, 7, line_1, line_2, line_3, line_4, line_5, line_6, Middle_Point);
  /* 获得发送长度 */
  uint16_t len1 = UI_Get_Send_Data_Len(SendBuff);
  /* 发送 */
  printf_ui("%s", SendBuff);
  osDelay(100);

  /* 榴弹提示线下部分 */
  Line_Draw(&line_Mid1, "011", UI_Graph_ADD, 8, Color, 1, 961, 530, 961, 330); //中垂线
  Line_Draw(&line_Mid2, "012", UI_Graph_ADD, 8, Color, 1, 960, 530, 960, 330); //中垂线偏移
  Line_Draw(&line_A1, "013", UI_Graph_ADD, 8, Color, 1, 860, 430, 1060, 430);  //A抛物线
  Line_Draw(&line_A2, "014", UI_Graph_ADD, 8, Color, 1, 860, 429, 1060, 429);  //A抛物线扶正
  Line_Draw(&line_B1, "015", UI_Graph_ADD, 8, Color, 1, 900, 380, 1020, 380);  //B抛物线
  Line_Draw(&line_B2, "016", UI_Graph_ADD, 8, Color, 1, 900, 379, 1020, 379);  //B抛物线扶正
  Line_Draw(&line_C1, "017", UI_Graph_ADD, 8, Color, 1, 920, 330, 1000, 330);  //C抛物线
                                                                               /* 将更新数据写入 */
  UI_ReFresh(SendBuff, 7, line_Mid1, line_Mid2, line_A1, line_A2, line_B1, line_B2, line_C1);
  /* 获得发送长度 */
  uint16_t len2 = UI_Get_Send_Data_Len(SendBuff);
  /* 在此填入发送语句 */
  printf_ui("%s", SendBuff);
  osDelay(100);
}

void FrictionWheelSpeed(void)
{
}

void Super_Capacitance(void)
{
  //Float_Draw(&SuperCapacitors, "021", UI_Graph_ADD, 0, UI_Color_Cyan, 12, 1, 2, 800, 540, UI_JUDGE_DATA);
  //UI_ReFresh(SendBuff, 1, SuperCapacitors);
  /* 发送 */
}


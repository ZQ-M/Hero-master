/*
 * @brief HLLս��-Ӣ�۲�����UI
 * @note  �����ƻ����뾲̬UI:����ʾ�ߣ�
 *                   ��̬UI:Ħ����ת�٣���һ��ǹ���ٶ�,��������
 * 
*/
#include "client_ui_task.h"

/* ����֡��������113 */
static uint8_t SendBuff[113];
/* �趨��̬���ݵķ����� */
const int UI_SEND_DELAY_TIME_MS = 1000;

/* ���Ħ�������� */
static const uint8_t friction_motor_num = 2;
static Motor_Measure_t friction_motor_feedback_data[friction_motor_num];
/* ��ò���ϵͳ���� */
const static Judge_data_t *UI_JUDGE_DATA;
/* ��ȡң�������� */
const static Rc_Ctrl_t *UI_RC_DATA;

/* ����UI�ṹ��--����ʾ���ϲ��� */
Graph_Data line_1, line_2, line_3, line_4, line_5, line_6, Middle_Point;
/* ����UI�ṹ��--����ʾ���²��� */
Graph_Data line_Mid1, line_Mid2, line_A1, line_A2, line_B1, line_B2, line_C1;
/* ����UI�ṹ��--��һ��ǹ���ٶȣ��������� */
Graph_Data MuzzleSpeed, SuperCapacitors;

/* �ж��Ǻ췽������������������ID */
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
    // /* ���û�����ID */
    // UI_Set_Comparable_Id(HERO_IN_WHICH_Camp);
    // /* ͨ���ֶ�������ĸ���;�̬UI - ����ʾ�� */
    // GrenadeCueLine(UI_Color_Yellow);
    // /* �Զ����¶�̬UI - ����ʾ�� */
    // Super_Capacitance();

    osDelay(UI_SEND_DELAY_TIME_MS);
  }
}


/*
 * @brief ��̬UI - �й�������ʾ��
 * 
 * @param Color UI_Color_Main 
                UI_Color_Yellow
                UI_Color_Green 
                UI_Color_Orange
                UI_Color_Purpli
                UI_Color_Pink 
                UI_Color_Cyan //����ɫ
                UI_Color_Black 
                UI_Color_White 
*/
void GrenadeCueLine(uint8_t Color)
{
  /* ����ʾ���ϲ��� */
  Line_Draw(&line_1, "001", UI_Graph_ADD, 9, Color, 2, 961, 550, 961, 600);       //��
  Line_Draw(&line_2, "002", UI_Graph_ADD, 9, Color, 2, 960, 550, 960, 600);       //��ƫ��
  Line_Draw(&line_3, "003", UI_Graph_ADD, 9, Color, 2, 950, 541, 900, 541);       //��
  Line_Draw(&line_4, "004", UI_Graph_ADD, 9, Color, 2, 950, 540, 900, 540);       //��ƫ��
  Line_Draw(&line_5, "005", UI_Graph_ADD, 9, Color, 2, 970, 541, 1020, 541);      //��
  Line_Draw(&line_6, "006", UI_Graph_ADD, 9, Color, 2, 970, 540, 1020, 540);      //��ƫ��
  Line_Draw(&Middle_Point, "007", UI_Graph_ADD, 9, Color, 2, 960, 540, 961, 541); //��ƫ��
  /* ����������д�� */
  UI_ReFresh(SendBuff, 7, line_1, line_2, line_3, line_4, line_5, line_6, Middle_Point);
  /* ��÷��ͳ��� */
  uint16_t len1 = UI_Get_Send_Data_Len(SendBuff);
  /* ���� */
  printf_ui("%s", SendBuff);
  osDelay(100);

  /* ����ʾ���²��� */
  Line_Draw(&line_Mid1, "011", UI_Graph_ADD, 8, Color, 1, 961, 530, 961, 330); //�д���
  Line_Draw(&line_Mid2, "012", UI_Graph_ADD, 8, Color, 1, 960, 530, 960, 330); //�д���ƫ��
  Line_Draw(&line_A1, "013", UI_Graph_ADD, 8, Color, 1, 860, 430, 1060, 430);  //A������
  Line_Draw(&line_A2, "014", UI_Graph_ADD, 8, Color, 1, 860, 429, 1060, 429);  //A�����߷���
  Line_Draw(&line_B1, "015", UI_Graph_ADD, 8, Color, 1, 900, 380, 1020, 380);  //B������
  Line_Draw(&line_B2, "016", UI_Graph_ADD, 8, Color, 1, 900, 379, 1020, 379);  //B�����߷���
  Line_Draw(&line_C1, "017", UI_Graph_ADD, 8, Color, 1, 920, 330, 1000, 330);  //C������
                                                                               /* ����������д�� */
  UI_ReFresh(SendBuff, 7, line_Mid1, line_Mid2, line_A1, line_A2, line_B1, line_B2, line_C1);
  /* ��÷��ͳ��� */
  uint16_t len2 = UI_Get_Send_Data_Len(SendBuff);
  /* �ڴ����뷢����� */
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
  /* ���� */
}


#include "can2.h"

/**
 * @brief           初始化 CAN2
 * @param[in]       void
 * @retval          void
 */
void Can2_Filter_Init(void)
{
    CAN_FilterTypeDef can_filter_st;

	can_filter_st.FilterActivation = ENABLE;
	can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
	can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
	can_filter_st.FilterIdHigh = 0x0000;
	can_filter_st.FilterIdLow = 0x0000;
	can_filter_st.FilterMaskIdHigh = 0x0000;
	can_filter_st.FilterMaskIdLow = 0x0000; // every frame will receive
	can_filter_st.FilterBank = 14;
	can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
	HAL_CAN_ConfigFilter(&hcan2, &can_filter_st);
	HAL_CAN_Start(&hcan2);
}

#include "FreeRTOS.h"
#include "task.h"
void Can2_Send_4Msg(uint32_t id, int16_t data1, int16_t data2, int16_t data3, int16_t data4)
{
    static CAN_TxHeaderTypeDef can_tx_msg_format;
	static uint8_t can_tx_msg[8];
	static uint32_t send_mail_box;
	can_tx_msg_format.StdId = id;
	can_tx_msg_format.IDE = CAN_ID_STD;
	can_tx_msg_format.RTR = CAN_RTR_DATA;
	can_tx_msg_format.DLC = 0x08;
	can_tx_msg[0] = data1 >> 8;
	can_tx_msg[1] = data1;
	can_tx_msg[2] = data2 >> 8;
	can_tx_msg[3] = data2;
	can_tx_msg[4] = data3 >> 8;
	can_tx_msg[5] = data3;
	can_tx_msg[6] = data4 >> 8;
	can_tx_msg[7] = data4;

	///< use HAL function send
	HAL_CAN_AddTxMessage(&hcan2, &can_tx_msg_format, can_tx_msg, &send_mail_box);
}

/**
 * @brief           CAN2上PITCH轴电机
 * @param[in]		id 		Can_id 
 * @param[in]		speed	电机速度 from -1279 ~ 1279
 * @param[in]		acc		电机加速度	from 0 ~ 255
 * @retval          void
 */
void Can2_Send_6Msg(uint32_t id, int32_t speed, uint8_t acc)
{
  static CAN_TxHeaderTypeDef can_tx_msg_format;
	static uint8_t can_tx_msg[8];
	static uint32_t send_mail_box;
	can_tx_msg_format.StdId = id;
	can_tx_msg_format.IDE = CAN_ID_STD;
	can_tx_msg_format.RTR = CAN_RTR_DATA;
	can_tx_msg_format.DLC = 0x05;
	unsigned char positive_or_negative = 0x00;
	if(speed < 0)
	{
		positive_or_negative = 0x01;
		speed = -speed;
	}
	int16_t speedi16 = speed;
	uint16_t speedu16 = speedi16;
	can_tx_msg[0] = 0xF6;
	can_tx_msg[1] = (positive_or_negative << 4) & 0xF0 + (speedu16 >> 8) & 0x0F;
	can_tx_msg[2] = speedu16 & 0x00FF;
	can_tx_msg[3] = acc;
	can_tx_msg[4] = 0x6B;

	///< use HAL function send
	HAL_CAN_AddTxMessage(&hcan2, &can_tx_msg_format, can_tx_msg, &send_mail_box);
}

#include "remote.h"

/**
 * @brief                                   解析串口接收的遥控器数据
 * @param[in]  {volatile_const_uint8_t_*}   sbus_buf 存储串口接收的数据缓冲区
 * @param[in]  {Rc_Ctrl_t_*}                rc_ctrl 指向遥控器的结构体指针
 * @retval                                  void
 */
static void Parse_Remoter_Data(volatile const uint8_t *sbus_buf, Rc_Ctrl_t *rc_ctrl)
{
	rc_ctrl->rc.ch0 = (sbus_buf[0] | (sbus_buf[1] << 8)) & 0x07ff;		  ///< Channel 0
	rc_ctrl->rc.ch1 = ((sbus_buf[1] >> 3) | (sbus_buf[2] << 5)) & 0x07ff; ///< Channel 1
	rc_ctrl->rc.ch2 = ((sbus_buf[2] >> 6) | (sbus_buf[3] << 2) |		  ///< Channel 2
					   (sbus_buf[4] << 10)) &
					  0x07ff;
	rc_ctrl->rc.ch3 = ((sbus_buf[4] >> 1) | (sbus_buf[5] << 7)) & 0x07ff; ///< Channel 3
	rc_ctrl->rc.s1 = ((sbus_buf[5] >> 4) & 0x0003);						  ///< Switch left
	rc_ctrl->rc.s2 = ((sbus_buf[5] >> 4) & 0x000C) >> 2;				  ///< Switch right
	rc_ctrl->mouse.x = sbus_buf[6] | (sbus_buf[7] << 8);				  ///< Mouse X axis
	rc_ctrl->mouse.y = sbus_buf[8] | (sbus_buf[9] << 8);				  ///< Mouse Y axis
	rc_ctrl->mouse.z = sbus_buf[10] | (sbus_buf[11] << 8);				  ///< Mouse Z axis
	rc_ctrl->mouse.press_l = sbus_buf[12];								  ///< Mouse Left Is Press ?
	rc_ctrl->mouse.press_r = sbus_buf[13];								  ///< Mouse Right Is Press ?
	rc_ctrl->key.value = sbus_buf[14] | (sbus_buf[15] << 8);			  ///< KeyBoard value
	rc_ctrl->rc.ch4 = sbus_buf[16] | (sbus_buf[17] << 8);				  ///< Channel 4

	rc_ctrl->rc.ch0 -= RC_CH_VALUE_OFFSET;
	rc_ctrl->rc.ch1 -= RC_CH_VALUE_OFFSET;
	rc_ctrl->rc.ch2 -= RC_CH_VALUE_OFFSET;
	rc_ctrl->rc.ch3 -= RC_CH_VALUE_OFFSET;
	rc_ctrl->rc.ch4 -= RC_CH_VALUE_OFFSET;
	//Wasd_Key_To_Virtual_Rocker(rc_ctrl，);
}

/**
 * @brief                   检验接收到的数据长度
 * @param[in]  {uint16_t}  本次接收到的数据长度
 * @retval                  1: 不合法
 *                          0: 合法
 */
static uint8_t Rc_Data_Len_Check(uint16_t len)
{
#define RC_EFFICTIVE_DATA_LEN 18
	if (len == RC_EFFICTIVE_DATA_LEN)
	{
		return 1;
	}
	else
	{
		return 0;
	}
#undef RC_EFFICTIVE_DATA_LEN
}

/**
 * @brief                   检验遥控器数据是否合法
 * @param[in]  {Rc_Ctrl_t}  遥控器数据指针
 * @retval                  1: 不合法
 *                          0: 合法
 */
static uint8_t Remoter_Data_Value_Check(Rc_Ctrl_t *rc)
{
	if (ROCKER_DATA_CHECK(rc->rc.ch0))
	{
		return 0;
	}
	if (ROCKER_DATA_CHECK(rc->rc.ch1))
	{
		return 0;
	}
	if (ROCKER_DATA_CHECK(rc->rc.ch2))
	{
		return 0;
	}
	if (ROCKER_DATA_CHECK(rc->rc.ch3))
	{
		return 0;
	}
	if (SWITCH_DATA_CHECK(rc->rc.s1))
	{
		return 0;
	}
	if (SWITCH_DATA_CHECK(rc->rc.s2))
	{
		return 0;
	}
	return 1;
}

/**
 * @brief               					解析遥控器数据并检验
 * @param[in] {volatile_const_uint8_t}   	遥控器数据缓冲区
 * @param[in] {Rc_Ctrl_t}   				遥控器数据指针结构体
 * @param[in] {uint16_t}					本次接收到的数据长度
 * @retval    {uint8_t}						检验结果
 */
uint8_t Rc_Data_Check_Parse(volatile const uint8_t *sbus_buf, Rc_Ctrl_t *rc_ctrl, uint16_t rc_data_len)
{
	if (!Rc_Data_Len_Check(rc_data_len))
	{
		return 0;
	}
	Parse_Remoter_Data(sbus_buf, rc_ctrl);
	return Remoter_Data_Value_Check(rc_ctrl);
}

/**
 * @brief               将 rc2 赋值给 rc1 
 * @param {Rc_Ctrl_t}   遥控器数据 1
 * @param {Rc_Ctrl_t}   遥控器数据 2
 * @retval              void
 */
void Rc_Data_Copy(Rc_Ctrl_t *rc1, Rc_Ctrl_t *rc2)
{
	rc1->key.value = rc2->key.value;

	rc1->mouse.press_l = rc2->mouse.press_l;
	rc1->mouse.press_r = rc2->mouse.press_r;
	rc1->mouse.x = rc2->mouse.x;
	rc1->mouse.y = rc2->mouse.y;
	rc1->mouse.z = rc2->mouse.z;

	rc1->rc.ch0 = rc2->rc.ch0;
	rc1->rc.ch1 = rc2->rc.ch1;
	rc1->rc.ch2 = rc2->rc.ch2;
	rc1->rc.ch3 = rc2->rc.ch3;
	rc1->rc.ch4 = rc2->rc.ch4;
	rc1->rc.s1 = rc2->rc.s1;
	rc1->rc.s2 = rc2->rc.s2;
}

/**
  * @brief          遥控器的死区判断，因为遥控器的拨杆在中位的时候，不一定为0，
  * @param          输入的遥控器值
  * @param          输出的死区处理后遥控器值
  * @param          死区值
  */
#define rc_deadband_limit(input, dealine)        \
    {                                                    \
        if ((input) > (dealine) || (input) < -(dealine)) \
        {                                                \
                              \
        }                                                \
        else                                             \
        {                                                \
            (output) = 0;                                \
        }                                                \
    }

/**
 * @brief               重置遥控器数据 
 * @param {Rc_Ctrl_t}   遥控器数据
 * @retval              void
 */
void Rc_Data_Reset(Rc_Ctrl_t *rc)
{
	rc->key.value = 0;

	rc->mouse.press_l = 0;
	rc->mouse.press_r = 0;
	rc->mouse.x = 0;
	rc->mouse.y = 0;
	rc->mouse.z = 0;

	rc->rc.ch0 = 0;
	rc->rc.ch1 = 0;
	rc->rc.ch2 = 0;
	rc->rc.ch3 = 0;
	rc->rc.ch4 = 0;
	rc->rc.s1 = 3;
	rc->rc.s2 = 3;
}

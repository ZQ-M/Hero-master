#ifndef __REMOTE_H_
#define __REMOTE_H_

#include "stm32f4xx.h"

#define RC_CH_VALUE_MIN                 ((uint16_t)364)
#define RC_CH_VALUE_OFFSET              ((uint16_t)1024)
#define RC_CH_VALUE_MAX                 ((uint16_t)1684)

#define RC_SW_UP                        ((uint8_t)1)
#define RC_SW_MID                       ((uint8_t)3)
#define RC_SW_DOWN                      ((uint8_t)2)

#define ROCKER_DATA_CHECK(data)         ( (data > 660) || (data < -660))
#define SWITCH_DATA_CHECK(data)         (!((data == 1) || (data == 2) || (data == 3)))


/**
 * @brief           遥控器数据结构 
 * @details         通过串口 1 接收到的数据对遥控器结构体成员进行解析赋值
 */
typedef __packed struct
{
    __packed struct
    {
        int16_t ch0;
        int16_t ch1;
        int16_t ch2;
        int16_t ch3;
        int16_t ch4;
        uint8_t s1;
        uint8_t s2;
    } rc;

    __packed struct
    {
        int16_t x;
        int16_t y;
        int16_t z;
        uint8_t press_l;
        uint8_t press_r;
    } mouse;

    __packed struct
    {
        uint16_t value;
    } key;

} Rc_Ctrl_t;

uint8_t Rc_Data_Check_Parse(volatile const uint8_t *sbus_buf,Rc_Ctrl_t *rc_ctrl,uint16_t rc_data_len);
void Rc_Data_Copy(Rc_Ctrl_t* rc1, Rc_Ctrl_t* rc2);
void Rc_Data_Reset(Rc_Ctrl_t* rc);

#endif //__REMOTE_H_


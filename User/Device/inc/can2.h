#ifndef __CAN2_H_
#define __CAN2_H_

#include "can.h"

void Can2_Filter_Init(void);
void Can2_Send_4Msg(uint32_t id, int16_t data1, int16_t data2, int16_t data3, int16_t data4);

#endif //__CAN2_H_

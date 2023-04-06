#ifndef __MATH2_H_
#define __MATH2_H_

#include "stm32f4xx.h"
/* 斜坡函数结构体 */
typedef __packed struct
{
    float input;        //输入数据
    float out;          //输出数据
    float min_value;    //限幅最小值
    float max_value;    //限幅最大值
    float step; //时间间隔
} ramp_function_source_t;

void Int16_Constrain(int16_t *data, int16_t min_value, int16_t max_value);
int16_t Int16_Limit(int16_t data, int16_t min_value, int16_t max_value);

void Int16_Add_Limit(int16_t *data, int16_t add_value, int16_t max_value);
void Int16_Minus_Limit(int16_t *data, int16_t minus_value, int16_t min_value);
void Float_Constraion(float *data, float min_value, float max_value);

//电机过零处理
void Handle_Angle8191_PID_Over_Zero(float *tar, float *cur);
void Handle_Angle360_PID_Over_Zero(float *tar, float *cur);
float Inv_Sqrt(float x);
//从步兵移植
float Hex4_To_Float1(unsigned char *array);///<数组解析出float
uint16_t Uint8_t_Array_To_Uint16_t(uint8_t *array);
/* 斜坡函数 */
void ramp_init(ramp_function_source_t *ramp_source_type, float step,float limit);
void ramp_calc(ramp_function_source_t *ramp_source_type, float input);
#endif // __MATH2_H_

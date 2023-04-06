#include "math2.h"

/**
 * @brief int16类型变量大小限制
 * @param {int16_t*} 待限制的变量指针，会直接更改地址所存放的数据
 * @param {int16_t} min_value 最小值
 * @param {int16_t} max_value 最大值
 * @return {*}
 */
void Int16_Constrain(int16_t *data, int16_t min_value, int16_t max_value)
{
	if (*data > max_value)
		*data = max_value;
	else if (*data < min_value)
		*data = min_value;
}

/**
 * @brief int16类型变量大小限制
 * @param {int16_t} data 待限制的变量
 * @param {int16_t} min_value 最小值
 * @param {int16_t} max_value 最大值
 * @return {int16_t} 限制后的数据
 */
int16_t Int16_Limit(int16_t data, int16_t min_value, int16_t max_value)
{
	if (data > max_value)
		return max_value;
	else if (data < min_value)
		return min_value;
	return data;
}

void Int16_Add_Limit(int16_t *data, int16_t add_value, int16_t max_value)
{
	*data += add_value;
	if (*data > max_value)
	{
		*data = max_value;
	}
}

void Int16_Minus_Limit(int16_t *data, int16_t minus_value, int16_t min_value)
{
	*data -= minus_value;
	if (*data < min_value)
	{
		*data = min_value;
	}
}
/**
 * @brief			float数据上下限幅
 * @param [in]		数据值；上下限幅
 * @return			void
*/
void Float_Constraion(float *data, float min_value, float max_value)
{
	if (*data > max_value)
	{
		*data = max_value;
	}
	else if (*data < min_value)
	{
		*data = min_value;
	}
}

/* 角度Pid时，在获取tar和cur之后紧接着调用

云台角度过零处理	0~8191
 */
void Handle_Angle8191_PID_Over_Zero(float *tar, float *cur)
{
	if(*tar - *cur > 4096)   //4096 ：半圈机械角度, 如果设定值和目标值之间的角度差已经超过了半圈
	{
		*cur += 8192;
	}
	else if(*tar - *cur < -4096)
	{
		*cur = *cur - 8192;
	}
}

/* 角度Pid时，在获取tar和cur之后紧接着调用

*云台角度过零处理，防止疯转	0~360
*/
void Handle_Angle360_PID_Over_Zero(float *tar, float *cur)
{
	if(*tar - *cur > 180)   //4096 ：半圈机械角度
	{
		*cur += 360;
	}
	else if(*tar - *cur < -180)
	{
		*cur = *cur - 360;
	}
}

/**
  * @brief  fast inverse square-root, to calculate 1/Sqrt(x)
  * @param  x: the number need to be calculated
  * @retval 1/Sqrt(x)
  */
float Inv_Sqrt(float x)
{
	float halfx = 0.5f * x;
	float y     = x;
	long  i     = *(long*)&y;

	i = 0x5f3759df - (i >> 1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));

	return y;
}

//数组解析出float
float Hex4_To_Float1(unsigned char *array)
{
	unsigned char array_copy[4];
	float f;

	for(unsigned int i=0; i<4; i++)
	{
		array_copy[i]=array[i];
	}
	f = *((float *)((array_copy)));
	return f;
}
/**
 * @brief					数据类型转换(用于裁判系统2字节数据解析)
 * @param [in] {uint8_t}
 * @return	{uint16_t}
 *
*/
uint16_t Uint8_t_Array_To_Uint16_t(uint8_t *array)
{
	return ( (array[1]<<8) | array[0] );
}

/**
  * @brief          斜波函数初始化
  * @author         RM
  * @param[in]      斜波函数结构体
  * @param[in]      间隔的时间，单位 s
  * @param[in]      最大值
  * @param[in]      最小值
  * @retval         返回空
  */
void ramp_init(ramp_function_source_t *ramp_source_type, float step,float limit)
{
    ramp_source_type->step = step;
    ramp_source_type->max_value = limit;
    ramp_source_type->min_value = -limit;
    ramp_source_type->input = 0.0f;
    ramp_source_type->out = 0.0f;
}

/**
  * @brief          斜波函数计算，根据输入的值进行叠加， 输入单位为 /s 即一秒后增加输入的值
  * @author         RM
  * @param[in]      斜波函数结构体
  * @param[in]      输入值
  * @param[in]      滤波参数
  * @retval         返回空
  */
void ramp_calc(ramp_function_source_t *ramp_source_type, float input)
{
    ramp_source_type->input = input;
    ramp_source_type->out += ramp_source_type->input * ramp_source_type->step;
    if (ramp_source_type->out > ramp_source_type->max_value)
    {
        ramp_source_type->out = ramp_source_type->max_value;
    }
    else if (ramp_source_type->out < ramp_source_type->min_value)
    {
        ramp_source_type->out = ramp_source_type->min_value;
    }
}

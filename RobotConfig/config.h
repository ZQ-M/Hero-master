/**
 * @file config.h
 * @brief   全局配置文件，包含各种调试，配置。后期移植所有参数调节
 * @version 0.1
 * @date    2022.3.5
 * @author  whale
 * @note    此文件仅做宏定义或参数设置，集中所有设置方便调试，具体的实现逻辑应在其他部分完成。
 * @copyright Copyright (c) 2022
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* 调试安全配置 */
#define LASER_OFF 1          ///<打开激光（调试时建议关闭防止伤人）
#define CHASSIS_SPEED_ZERO 0 ///<底盘无力模式,防止底盘疯转
#define GIMBAL_SPEED_ZERO 0  ///<云台无力模式,防止底盘疯转
#define LOSS_REMOTE_SIGNAL 1 ///<遥控器信号丢失检测以及错误处理（暂时未实现）

/* 机器人属性参数 */
/* Yaw */
#define YAW_GM6020_HEAD_ANGLE 3328 ///< 维修之后角度会变    2468
#define YAW_LIMIT_SPEED 150        ///<YAW轴速度限幅 

/* PID */

/* 调试模式 */
#define INFORMATION 1
#define DEBUG 0           ///< debug_printf()函数使能
#define ERROE_FOR_SHELL 0 ///<文件错误信息输出开关（暂未完成）

/* 添加机器人返回数据函数定义列表 */

typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

/* exact-width unsigned integer types */
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned char bool_t;
typedef float fp32;
typedef double fp64;

#endif

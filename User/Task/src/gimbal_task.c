#include "gimbal_task.h"
#include "externel_gyroscope_task.h"

/* 云台电机下标 */
static const uint8_t gimbal_motor_num = 2;  ///< 云台电机的数量
static const uint8_t yaw_motor_index = 0;   ///< yaw 轴电机在电机数据结构体中的下标
static const uint8_t pitch_motor_index = 1; ///< pitch 轴电机在电机数据结构体中的下标
/* Pitch角度限幅 */
static const uint16_t pitch_up_angle_limit = 5200;   ///< pitch 轴云台最低角度
static const uint16_t pitch_middle_angle = 5630;     ///< pitch 轴云台中间角度
static const uint16_t pitch_down_angle_limit = 6200; ///< pitch 轴云台最高角度
/* Restart角度 */
static float yaw_angle_set = 0;                    ///< 这里初始角是0，因为在chassis_task.c中，对于跟随根据初始机械角度进行了处理
static float pitch_angle_set = pitch_middle_angle; ///< pitch 轴云台设置的角度
/* 指向接收数据 */
static CAN_RxHeaderTypeDef *can2_rx_header_pt;   ///< can2 接收的头数据结构体指针
static uint8_t *can2_rxd_data_buffer;            ///< can2 接收的数据存放的数组首地址
static Rc_Ctrl_t *rc_data_pt;                    ///< 指向遥控器数据的结构体指针
static Robot_control_data_t *robot_mode_data_pt; ///< 指向机器人模式的结构体指针
static Motor_Measure_t gimbal_motor_parsed_feedback_data[gimbal_motor_num]; ///< 解析后的云台电机数据数组(Yaw:0;Pitch:1)
static Wt61c_Data_t *imu_data_usart6;                                       ///< 指向解析后的串口陀螺仪数据

/* 存放pid计算之后的输出 */
float pid_out[ALL_PID] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; ///< 这个参数也可放在每个task.c中，定义为静态变量
/* 云台初始化函数声明 */
void Gimbal_Init(void);

void StartGimbalTask(void const *argument)
{
    Gimbal_Init();

    osDelay(1000);
    for (;;)
    {
        ///< 板放置上云台除Yaw轴其他两轴数据稳定数据稳定
        //< 所有的调试打印全部采用debug.printf()，在usart3.h中的宏定义控制条件编译，关闭宏之后debug_printf指向空宏

        /*选择操作设备*/
        if (robot_mode_data_pt->mode.control_device == remote_controller_device_ENUM) ///<遥控器模式
        {
            // 底盘云台模式: 1底盘跟随 2小陀螺  3垂稳云台 4垂稳+小陀螺 5特殊模式
            switch (robot_mode_data_pt->mode.rc_motion_mode)
            {

            case 1: ///<底盘跟随
            {
                /*底盘跟随模式和小陀螺模式在云台任务中逻辑共用,
                均是以陀螺仪做角度闭环作为速度值，差别在底盘任务*/
            }

            case 2: ///<底盘小陀螺+云台自由运动
            {
                pitch_angle_set -= (rc_data_pt->rc.ch1) / 12.0f; ///< 改之前是*10.0f
                /*pitch轴角度限幅*/
                Pitch_Angle_Limit(&pitch_angle_set, pitch_down_angle_limit, pitch_up_angle_limit);
                yaw_angle_set -= (rc_data_pt->rc.ch0) / 160.0f;
                // yaw轴设定值角度回环
                if (yaw_angle_set > 360)
                {
                    yaw_angle_set -= 360;
                }
                if (yaw_angle_set < 0)
                {
                    yaw_angle_set += 360;
                }
                pid_out[Yaw_target_Speed] = Calc_Yaw_Angle360_Pid(yaw_angle_set, imu_data_usart6->angle.yaw_z);
                pid_out[Pitch_target_Speed] = Calc_Pitch_Angle8191_Pid(pitch_angle_set, &gimbal_motor_parsed_feedback_data[pitch_motor_index]);
                break;
            }
            case 3: ///< 自稳+云台自由移动
            {
                /* 跟随与小陀螺在云台共用逻辑 */
            }
            case 4: ///< 自稳+小陀螺
            {

                pitch_angle_set -= rc_data_pt->rc.ch1 / 160.0f;
                yaw_angle_set -= (rc_data_pt->rc.ch0) / 160.0f; ///< 倍率待调整
                /* 用陀螺仪做角度闭环，用机械角做限位 */
                Pitch_Angle_Limit(&pitch_angle_set, pitch_down_angle_limit, pitch_up_angle_limit);
                if (yaw_angle_set > 360)
                {
                    yaw_angle_set -= 360;
                }
                if (yaw_angle_set < 0)
                {
                    yaw_angle_set += 360;
                }

                pid_out[Yaw_target_Speed] = Calc_Yaw_Angle360_Pid(yaw_angle_set, imu_data_usart6->angle.yaw_z);
                pid_out[Pitch_target_Speed] = Calc_Pitch_Angle8191_Imu_Pid(pitch_angle_set, imu_data_usart6->angle.pitch_y);
                break;
            }
            case 5: ///<特殊模式
            {

                pitch_angle_set -= (rc_data_pt->rc.ch1) / 12.0f;
                Pitch_Angle_Limit(&pitch_angle_set, pitch_up_angle_limit, pitch_down_angle_limit); ///< pitch角度限幅
                /* 以下是特殊模式的yaw轴 */
                pid_out[Yaw_target_Speed] = -(rc_data_pt->rc.ch0) / 6.0f;
                pid_out[Pitch_target_Speed] = Calc_Pitch_Angle8191_Pid(pitch_angle_set, &gimbal_motor_parsed_feedback_data[pitch_motor_index]);
                break;
            }

            default:
            {
                break;
            }
            }
        }
        else if (robot_mode_data_pt->mode.control_device == mouse_keyboard_device_ENUM) ///<键鼠模式
        {
            switch (robot_mode_data_pt->mode.mouse_keyboard_gimbal_mode)
            {
            case 1: ///<手动模式（云台控制数据来自鼠标移动）

            {
                /*目标角度设定*/
                yaw_angle_set -= robot_mode_data_pt->virtual_rocker.ch0 / 58.0f; ///<倍率需要调整。这个数据是步兵的
                pitch_angle_set -= robot_mode_data_pt->virtual_rocker.ch1 / 1.6f;

                // yaw角度回环
                if (yaw_angle_set > 360)
                {
                    yaw_angle_set -= 360;
                }
                if (yaw_angle_set < 0)
                {
                    yaw_angle_set += 360;
                }
                /*pitch轴角度限幅*/
                Pitch_Angle_Limit(&pitch_angle_set, pitch_down_angle_limit, pitch_up_angle_limit);

                /*两轴串级PID的角度环计算*/
                pid_out[Pitch_target_Speed] = Calc_Pitch_Angle8191_Pid(pitch_angle_set, &gimbal_motor_parsed_feedback_data[pitch_motor_index]);
                pid_out[Yaw_target_Speed] = Calc_Yaw_Angle360_Pid(yaw_angle_set, imu_data_usart6->angle.yaw_z);
            }

            break;

            case 2: ///<自稳云台模式
            {
                /*目标角度设定*/
                yaw_angle_set -= robot_mode_data_pt->virtual_rocker.ch0 / 58.0f; ///<倍率需要调整。这个数据是步兵的
                pitch_angle_set -= robot_mode_data_pt->virtual_rocker.ch1 / 1.6f;

                // yaw角度回环
                if (yaw_angle_set > 360)
                {
                    yaw_angle_set -= 360;
                }
                if (yaw_angle_set < 0)
                {
                    yaw_angle_set += 360;
                }
                /*pitch轴角度限幅*/
                Pitch_Angle_Limit(&pitch_angle_set, pitch_down_angle_limit, pitch_up_angle_limit);

                /*两轴串级PID的角度环计算*/
                pid_out[Pitch_target_Speed] = Calc_Pitch_Angle8191_Pid(pitch_angle_set, &gimbal_motor_parsed_feedback_data[pitch_motor_index]);
                /* 上一句需重写角度环PID计算函数，pitch轴使用陀螺仪数据 */
                pid_out[Yaw_target_Speed] = Calc_Yaw_Angle360_Pid(yaw_angle_set, imu_data_usart6->angle.yaw_z);
                break;
            }
            case 3: ///<特殊模式（以云台坐标系为整车运动坐标系，前后左右的运动均以云台视角为准，在Chassis_task.c有具体的说明）
            {
                pitch_angle_set += robot_mode_data_pt->virtual_rocker.ch1 / 3.0f;
                /*pitch角度限制*/
                Pitch_Angle_Limit(&pitch_angle_set, pitch_up_angle_limit, pitch_down_angle_limit);

                pid_out[Yaw_target_Speed] = robot_mode_data_pt->virtual_rocker.ch0 / 1.6f;
                pid_out[Pitch_target_Speed] = Calc_Pitch_Angle8191_Pid(pitch_angle_set, &gimbal_motor_parsed_feedback_data[pitch_motor_index]);
                break;
            }
            default:
                break;
            }
        }
        ///<云台串环速度环计算及发送底盘电机速度
        YAW_SPEED_OUTPUT_LIMIT(&pid_out[Yaw_target_Speed], YAW_LIMIT_SPEED); // yaw轴速度限制

#if GIMBAL_SPEED_ZERO //云台无力模式开关，在config.h
        {
            pid_out[Yaw_target_Speed] = 0;
            pid_out[Pitch_target_Speed] = 0;
        }
#endif
        /**
         * @brief   云台速度环计算以及CAN2发送电机数据
          */

        Set_Gimbal_Motors_Speed(
            -pid_out[Yaw_target_Speed],
            pid_out[Pitch_target_Speed],
            gimbal_motor_parsed_feedback_data[yaw_motor_index].speed_rpm,
            gimbal_motor_parsed_feedback_data[pitch_motor_index].speed_rpm);

        osDelay(20);
    }
}

/**
 * @brief               云台相关的所有初始化
 *                      CAN2初始化，控制数据接收初始化，陀螺仪数据接收初始化
 * @param [in]          void
 * @return              void
 */
void Gimbal_Init(void)
{
    Can2_Filter_Init();

    can2_rx_header_pt = Get_CAN2_Rx_Header();
    can2_rxd_data_buffer = Get_CAN2_Rxd_Buffer();
    rc_data_pt = Get_Rc_Parsed_RemoteData_Pointer();
    robot_mode_data_pt = Get_Parsed_RobotMode_Pointer();
    imu_data_usart6 = Get_Gyroscope_Data_t();
    gimbal_motor_parsed_feedback_data[yaw_motor_index] = Get_Yaw_Data();
    gimbal_motor_parsed_feedback_data[pitch_motor_index] = Get_Pitch_Data();
    __OPEN_CAN2_RX_FIFO0_IT__; ///<打开CAN接收中断
}

/**
  * @brief   封装以供中断调用（解析进中断的暂时解决方案）
  * @note    后续需要把变量移植到can2_device.c中重构解析
 */
void Can2_Parse_For_Callback(void)
{
    Parse_Can2_Gimbal_Rxd_Data(can2_rx_header_pt, can2_rxd_data_buffer, gimbal_motor_parsed_feedback_data);
}
/**
 * @brief                       解析 CAN2 解析云台接收到的两个云台的数据并进行解析
 * @param p_can_rx_header       指向接收 CAN2 数据头的结构体指针
 * @param data                  接收到的 8 Byte 数据
 * @param motor                 传进来的电机结构体数组首地址
 * @retval                      void
 */
void Parse_Can2_Gimbal_Rxd_Data(CAN_RxHeaderTypeDef *p_can_rx_header, uint8_t *data, Motor_Measure_t *motor)
{

    switch (p_can_rx_header->StdId)
    {
    case CAN_YAW_MOTOR_ID:
    {
        Calculate_Motor_Data(&gimbal_motor_parsed_feedback_data[yaw_motor_index], data);
        break;
    }

    case CAN_PITCH_MOTOR_ID:
    {
        Calculate_Motor_Data(&gimbal_motor_parsed_feedback_data[pitch_motor_index], data);
        break;
    }

    default:
    {
        break;
    }
    }
}

/**
 * @brief                       获取解析后的云台电机数据
 * @param[in]                   void
 * @return {Motor_Measure_t*}   解析后的云台电机数据（数组首地址）
 */
Motor_Measure_t *Get_Gimbal_Parsed_FeedBack_Data(void)
{
    return gimbal_motor_parsed_feedback_data;
}

/**
 * @brief                       获取 pitch 轴电机在数组中的下标
 * @param[in]                   void
 * @return {const_uint8_t*}     下标
 */
const uint8_t *Get_Pitch_Motor_Index(void)
{
    return &pitch_motor_index;
}

/**
 * @brief                       获取 yaw 轴电机在数组中的下标
 * @param[in]                   void
 * @return {const_uint8_t*}     下标
 */
const uint8_t *Get_Yaw_Motor_Index(void)
{
    return &yaw_motor_index;
}

/**
 * @brief                       将 gm6020 的电机转子值换算成角度
 * @param[in] {gm6020_angle}    gm6020 的电机转子值
 * @return {float}              角度值
 */
float GM6020_YAW_Angle_To_360(uint16_t gm6020_angle)
{
#define ROBOT_HEAD_ANGLE YAW_GM6020_HEAD_ANGLE
    int16_t yaw_angle = gm6020_angle - ROBOT_HEAD_ANGLE;
    if (yaw_angle < 0)
    {
        yaw_angle += 8191;
    }
    return (float)(yaw_angle * 360 / 8191);
#undef ROBOT_HEAD_ANGLE
}

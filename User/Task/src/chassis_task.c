/**
 * @file chassis_task.c
 * @brief 底盘任务
 * @version 0.1
 * @date 2021-9-28
 * @copyright Copyright (c) 2021
 */
#include "chassis_task.h"
/* 宏定义 */
#define CHASSIS_SPEED_ZERO 0
#define OUTPUT_LIMIT(data, limit) Float_Constraion(data, -limit, limit) ///< 输出限幅
#define CHASSIS_MOTOR_DEFAULT_BASE_RATE 5.5f                            ///>键鼠操作中，底盘的速度倍率
#define CHASSIS_MOTOR_GYRO_BASE_RATE 5.0f                               ///>底盘小陀螺的速度倍率

/* 数据指针 */
static Rc_Ctrl_t *rc_data_pt;                               ///< 指向解析后的遥控器结构体指针
static Robot_control_data_t *robot_mode_data_pt;            ///< 指向解析后的机器人模式结构体指针(也包括了虚拟键鼠通道的值)
static Motor_Measure_t *chassis_motor_feedback_parsed_data; ///< 指向解析后的底盘电机数据
static Motor_Measure_t *gimbal_motor_feedback_parsed_data;  ///< 指向解析后的云台电机数据
static const uint8_t *yaw_motor_index;                      ///< 指向yaw 轴电机在云台电机数据中的下标
static const uint8_t *pitch_motor_index;                    ///< 指向pitch 轴电机在云台电机数据中的下标
static const Judge_data_t *referee_date_pt;                 ///< 指向解析后的裁判系统数据

/* 限幅 */
static float chassis_motor_boost_rate = 1.0f; ///< 底盘电机倍率
/* PID参数实例化 */
static Pid_Position_t chassis_follow_pid = NEW_POSITION_PID(0.26, 0, 0.8, 5000, 500, 0, 1000, 500); ///< 底盘跟随PID

/* 移植自CAN1解析 */
static const uint8_t can1_motor_device_number = 4;
static uint8_t *can1_rxd_data;
static CAN_RxHeaderTypeDef *can1_rx_header;
static uint8_t *can1_rxd_data;
static Motor_Measure_t m3508_feddback_data[can1_motor_device_number]; ///<类型为解析后的电机数据结构体，后标为CAN1电机编号1~4

/* 函数声明 */
void Chassis_Init(void);
static uint16_t Calc_Gyro_Speed_By_Power_Limit(uint16_t power_limit);
static float Calc_Chassis_Follow(void);
static void Calc_Gyro_Motors_Speed(float *motors_speed, float rotate_speed, float move_direction, float x_move_speed, float y_move_speed);

void StartChassisTask(void const *argument)
{
    static float motor_speed[4] = {0.0, 0.0, 0.0, 0.0}; ///< 待发送的电机速度值
    float follow_pid_output;                            ///< 跟随PID输出

    Chassis_Init(); ///<底盘初始化

    osDelay(500);

    for (;;)
    {
        HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
        ///<选择操作设备    键鼠&遥控器
        if (robot_mode_data_pt->mode.control_device == mouse_keyboard_device_ENUM) ///< 键鼠模式
        {
            switch (robot_mode_data_pt->mode.mouse_keyboard_chassis_mode)
            {
            case mk_chassis_follow_mode_ENUM: ///<底盘跟随
            {
                follow_pid_output = Calc_Chassis_Follow(); ///< 计算跟随PID输出

#if 0 //0开启防滑(ESP) 1关闭防滑
						motor_speed[0] = robot_mode_data_pt->virtual_rocker.ch2 + robot_mode_data_pt->virtual_rocker.ch3 + follow_pid_output + robot_mode_data_pt->mouse.x/0.38f;
						motor_speed[1] = robot_mode_data_pt->virtual_rocker.ch2 - robot_mode_data_pt->virtual_rocker.ch3 + follow_pid_output + robot_mode_data_pt->mouse.x/0.38f;
						motor_speed[2] = -robot_mode_data_pt->virtual_rocker.ch2 + robot_mode_data_pt->virtual_rocker.ch3 + follow_pid_output + robot_mode_data_pt->mouse.x/0.38f;
						motor_speed[3] = -robot_mode_data_pt->virtual_rocker.ch2 - robot_mode_data_pt->virtual_rocker.ch3 + follow_pid_output + robot_mode_data_pt->mouse.x/0.38f;
#else
                Calc_Gyro_Motors_Speed(motor_speed,
                                       0,
                                       GM6020_YAW_Angle_To_360(gimbal_motor_feedback_parsed_data[*yaw_motor_index].mechanical_angle),
                                       robot_mode_data_pt->virtual_rocker.ch3,
                                       robot_mode_data_pt->virtual_rocker.ch2);

                motor_speed[0] += follow_pid_output + robot_mode_data_pt->virtual_rocker.ch0 / 0.38f;
                motor_speed[1] += follow_pid_output + robot_mode_data_pt->virtual_rocker.ch0 / 0.38f;
                motor_speed[2] += follow_pid_output + robot_mode_data_pt->virtual_rocker.ch0 / 0.38f;
                motor_speed[3] += follow_pid_output + robot_mode_data_pt->virtual_rocker.ch0 / 0.38f;
#endif

                motor_speed[0] *= (float)(CHASSIS_MOTOR_DEFAULT_BASE_RATE * chassis_motor_boost_rate);
                motor_speed[1] *= (float)(CHASSIS_MOTOR_DEFAULT_BASE_RATE * chassis_motor_boost_rate);
                motor_speed[2] *= (float)(CHASSIS_MOTOR_DEFAULT_BASE_RATE * chassis_motor_boost_rate);
                motor_speed[3] *= (float)(CHASSIS_MOTOR_DEFAULT_BASE_RATE * chassis_motor_boost_rate);

                break;
            }

            case mk_chassis_gyro_mode_ENUM: ///<底盘小陀螺

            {
                ///< 计算小陀螺时的电机速度
                Calc_Gyro_Motors_Speed(motor_speed,
                                       Calc_Gyro_Speed_By_Power_Limit(referee_date_pt->power_heat_data.chassis_power),
                                       GM6020_YAW_Angle_To_360(gimbal_motor_feedback_parsed_data[*yaw_motor_index].mechanical_angle),
                                       (float)robot_mode_data_pt->virtual_rocker.ch3 * CHASSIS_MOTOR_GYRO_BASE_RATE * chassis_motor_boost_rate,
                                       (float)robot_mode_data_pt->virtual_rocker.ch2 * CHASSIS_MOTOR_GYRO_BASE_RATE * chassis_motor_boost_rate);

                break;
            }

            case mk_chassis_special_mode_ENUM: ///<特殊模式（云台陀螺仪失效时使用）
            {
                Calc_Gyro_Motors_Speed(motor_speed,
                                       0,
                                       GM6020_YAW_Angle_To_360(gimbal_motor_feedback_parsed_data[*yaw_motor_index].mechanical_angle),
                                       robot_mode_data_pt->virtual_rocker.ch3 * 6.0f,
                                       robot_mode_data_pt->virtual_rocker.ch2 * 6.0f);

                break;
            }
            }
        }

        else if (robot_mode_data_pt->mode.control_device == remote_controller_device_ENUM) ///<遥控器控制
        {

            switch (robot_mode_data_pt->mode.rc_motion_mode)
            {
                //底盘跟随
            case 1:
            case 3:
            {
                follow_pid_output = Calc_Chassis_Follow();

                motor_speed[0] = rc_data_pt->rc.ch2 + rc_data_pt->rc.ch3 + follow_pid_output + rc_data_pt->rc.ch0 / 2.9f;
                motor_speed[1] = rc_data_pt->rc.ch2 - rc_data_pt->rc.ch3 + follow_pid_output + rc_data_pt->rc.ch0 / 2.9f;
                motor_speed[2] = -rc_data_pt->rc.ch2 + rc_data_pt->rc.ch3 + follow_pid_output + rc_data_pt->rc.ch0 / 2.9f;
                motor_speed[3] = -rc_data_pt->rc.ch2 - rc_data_pt->rc.ch3 + follow_pid_output + rc_data_pt->rc.ch0 / 2.9f;

                motor_speed[0] *= 8.5f;
                motor_speed[1] *= 8.5f;
                motor_speed[2] *= 8.5f;
                motor_speed[3] *= 8.5f;

                break;
            }

            //底盘小陀螺
            case 2:
            case 4:
            {
                Calc_Gyro_Motors_Speed(motor_speed,
                                       Calc_Gyro_Speed_By_Power_Limit(referee_date_pt->game_robot_status.chassis_power_limit),
                                       GM6020_YAW_Angle_To_360(gimbal_motor_feedback_parsed_data[*yaw_motor_index].mechanical_angle),
                                       rc_data_pt->rc.ch3 * 8.0f,
                                       rc_data_pt->rc.ch2 * 8.0f);
                break;
            }

            //特殊
            case 5:
            {
                Calc_Gyro_Motors_Speed(motor_speed,
                                       0,
                                       GM6020_YAW_Angle_To_360(gimbal_motor_feedback_parsed_data[*yaw_motor_index].mechanical_angle),
                                       rc_data_pt->rc.ch3 * 10.0f,
                                       rc_data_pt->rc.ch2 * 10.0f);
                break;
            }
            }
        }
        /* 额定转速 469rpm,减速箱减速比约为19:1 */
        /* 调试阶段速度限幅从8899改成889 */
        OUTPUT_LIMIT(&motor_speed[0], 8899);
        OUTPUT_LIMIT(&motor_speed[1], 8899);
        OUTPUT_LIMIT(&motor_speed[2], 8899);
        OUTPUT_LIMIT(&motor_speed[3], 8899);

//底盘关闭宏定义，在config.h
#if CHASSIS_SPEED_ZERO
        motor_speed[0] = 0;
        motor_speed[1] = 0;
        motor_speed[2] = 0;
        motor_speed[3] = 0;
#endif

        // uint8_t ATEST = Get_Judge_Data()->ext_game_robot_status_t.mains_power_chassis_output;
        // debug_print("%d\r\n",ATEST);

        /* 底盘上电判断 */
        if (referee_date_pt->ext_game_robot_status_t.mains_power_chassis_output == 1)
        // if (1)
        {
            /**
            * @brief   底盘速度PID环计算以及设置底盘速度
            */
            Set_ChassisMotor_Speed(motor_speed[0],
                                   motor_speed[1],
                                   motor_speed[2],
                                   motor_speed[3],
                                   chassis_motor_feedback_parsed_data);
        }
        else
        {
        Can1_Send_4Msg(CAN_CHASSIS_ALL_ID,0,0,0,0);
        }
        osDelay(5);
    }
}
void Chassis_Init(void)
{
    /* 解析 */
    can1_rxd_data = Get_CAN1_Rxd_Data();   ///< 返回CAN1收到的原始数据
    can1_rx_header = Get_CAN1_Rx_Header(); ///< 返回 CAN1 接收数据头结构体指针

    Can1_Filter_Init();                                                    ///< CAN1底层初始化
    referee_date_pt = Get_Judge_Data();                                    ///< 解析后的裁判系统数据
    rc_data_pt = Get_Rc_Parsed_RemoteData_Pointer();                       // 获取解析后的遥控器数据
    robot_mode_data_pt = Get_Parsed_RobotMode_Pointer();                   //机器人模式结构体指针
    chassis_motor_feedback_parsed_data = Get_Can1_Feedback_Data();         // CAN1 总线上电机的反馈数据
    gimbal_motor_feedback_parsed_data = Get_Gimbal_Parsed_FeedBack_Data(); // CAN2总线上电机的反馈数据
    yaw_motor_index = Get_Yaw_Motor_Index();                               // 获取 yaw 轴电机在数组中的下标
    pitch_motor_index = Get_Pitch_Motor_Index();                               // 获取 yaw 轴电机在数组中的下标
}

/**
 * @brief          计算小陀螺时，各个电机速度
 * @details        效果：底盘以设置速度小陀螺旋转或处于特殊状态，同时可以以云台枪管为头，前进后退、左右平移
 * @param[in]      {float *motors_speed }      储存电机速度的指针
 * @param[in]      {float rotate_speed }       旋转的电机速度
 * @param[in]      {float move_direction }     运动的方向角度0°~360°，以机器朝向为0°(云台GM6020电机相对底盘的角度)
 * @param[in]      {float x_move_speed}        x轴速度（X摇杆）
 * @param[in]      {float y_move_speed}        y轴速度（Y摇杆）
 * @retval                                     void
 */
void Calc_Gyro_Motors_Speed(float *motors_speed, float rotate_speed, float move_direction, float x_move_speed, float y_move_speed)
{
    ///< 判断角度是否错误
    if (move_direction > 360)
    {
        return;
    }

    ///< 角度换算弧度
    float move_radin = move_direction * 3.14159f / 180.0f;
    float radin_sin = sinf(move_radin); ///< sin(x)
    float radin_cos = cosf(move_radin);

    ///< 陀螺速度赋值
    motors_speed[0] = rotate_speed;
    motors_speed[1] = rotate_speed;
    motors_speed[2] = rotate_speed;
    motors_speed[3] = rotate_speed;

    ///< 计算速度增加值
    float x_x_speed = x_move_speed * radin_cos;
    float x_y_speed = x_move_speed * radin_sin;

    float y_x_speed = y_move_speed * radin_sin;
    float y_y_speed = y_move_speed * radin_cos;

    motors_speed[0] += ((x_x_speed - x_y_speed) + (y_x_speed + y_y_speed));
    motors_speed[1] += ((-x_x_speed - x_y_speed) + (-y_x_speed + y_y_speed));
    motors_speed[2] += ((x_x_speed + x_y_speed) + (y_x_speed - y_y_speed));
    motors_speed[3] += ((-x_x_speed + x_y_speed) + (-y_x_speed - y_y_speed));
}
/**
 * @author          bashpow
 * @brief           小陀螺底盘限制功率
 * @details         返回的是不同实时功率下的小陀螺底盘电机速度目标值
 * @note            后面与官方开源功率控制比较之后修改一下；还有就是功率计算要改成英雄的
 * @param [in]      通过裁判系统读取的功率
 * */
static uint16_t Calc_Gyro_Speed_By_Power_Limit(uint16_t power_limit)
{
    if (power_limit < 50)
    {
        return 1800;
    }
    else if (power_limit > 300)
    {
        return 2000;
    }
    else if (power_limit > 120)
    {
        return 6000;
    }

    return power_limit * 60;
}

/**
 * @brief           计算底盘跟随 pid
 * @param[in]       void
 * @return          float 计算出的速度值
 */
float Calc_Chassis_Follow(void)
{
#define YAW_INIT_ANGLE YAW_GM6020_HEAD_ANGLE

    float follow_tar = YAW_INIT_ANGLE;
    float follow_cur = gimbal_motor_feedback_parsed_data[*yaw_motor_index].mechanical_angle;
    float pitchoutpu1t = gimbal_motor_feedback_parsed_data[1].mechanical_angle;
    //debug_print("%.2f\r\n",pitchoutpu1t);
    Handle_Angle8191_PID_Over_Zero(&follow_tar, &follow_cur);

    return -(Pid_Position_Calc(&chassis_follow_pid, follow_tar, follow_cur));

#undef YAW_INIT_ANGLE
}

/**
 * @brief           解析 CAN1 上的反馈数据
 * @param[in]       void
 * @retval          void
 */
void Parse_Can1_Rxd_Data(void)
{
    static uint8_t i = 0;
    i = can1_rx_header->StdId - CAN_3508_M1_ID;
    Calculate_Motor_Data(&m3508_feddback_data[i], can1_rxd_data);
}

/**
 * @brief           解析 CAN1 上的PITCH编码器数据
 * @param[in]       void
 * @retval          void
 */
void Parse_Can1_Pitch_Encoding(void)
{
    gimbal_motor_feedback_parsed_data[*pitch_motor_index].mechanical_angle = (uint16_t)(can1_rxd_data[4] << 8 | can1_rxd_data[3]);
}

/**
 * @brief                       返回 CAN1 总线上电机的反馈数据
 * @param[in]                   void
 * @return {Motor_Measure_t*}   反馈电机数据结构体指针
 */
Motor_Measure_t *Get_Can1_Feedback_Data(void)
{
    return m3508_feddback_data;//不用加取地址，数组名就是指，而不是定义为结构体。与remote_task.c中不同。
}

/**
 * @brief                     返回 CAN1 总线上电机的数量
 * @param[in]                 void
 * @return {const uint8_t*}   CAN1 总线上电机数量
 */
const uint8_t *Get_Can1_Motor_DeviceNumber(void)
{
    return (&can1_motor_device_number);
}
/**
 * @brief           通知 CAN1 数据解析任务进行数据解析
 * 目前是解析CAN1四个底盘电机数据
 * @param[in]       void
 * @retval          void
 * @note            移植过来就不需要了
 */
void Info_Can1_ParseData_Task(void)
{
    Parse_Can1_Rxd_Data();
}

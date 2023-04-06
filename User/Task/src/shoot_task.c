/**
 * @file    shoot_task.c
 * @brief   发射机构，两个摩擦轮3508 电机的 id 分别为 201 和 202, 挂载在 can2 总线上
 *          解析后的数据分别放在 shoot_motor_feedback_data[0]、shoot_motor_feedback_data[1]
 * @note    暂未完成自适应摩擦轮速度
 * @version 0.1
 * @date    2021-10-02
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "shoot_task.h"

extern osThreadId waveWheelTaskHandle;

static Rc_Ctrl_t *rc_data_pt;                       //遥控器信息
static Robot_control_data_t *robot_control_data_pt; //机器人控制信息

static float fric_speed = 0; ///< 摩擦轮速度

static const uint8_t friction_motor_num = 2;
static Motor_Measure_t friction_motor_feedback_data[friction_motor_num];
static Motor_Measure_t *wave_motor_feedback_data;
static int *erroe_integral;
static uint16_t *last_machine_angle;
static uint16_t *this_machine_angle;
static CAN_RxHeaderTypeDef *can2_rx_header_p;
static uint8_t *can2_rxd_data_buffer;
const static Judge_data_t *judge_data;

/* 初始化 */
static int8_t is_ok_fire = 0; ///< 摩擦轮就绪标志

static int16_t last_wave_ch_value = 1;
static int16_t this_wave_ch_value = 1;

static const int32_t fire_one_bullet = 0x00000001; ///< 单发信号量
static const int32_t fire_five_bullet = 0x00000101;
static const uint32_t wave_once_machine_angle = 50000;
/* 移植射击控制结构体 */
shoot_control_t shoot_control; //射击数据
void shooter_init(void);
void shoot_init(void);
void StartShootTask(void const *argument)
{

    shooter_init();
    shoot_init();

    (void)rc_data_pt; ///< 避免 warning
    (void)judge_data;
    (void)last_machine_angle;
    (void)this_machine_angle;
    (void)wave_motor_feedback_data;
    (void)erroe_integral;
    (void)wave_once_machine_angle;

    osDelay(100);

    for (;;)
    {
        Parse_Friction_Wave_Motor_Feedback_Data(can2_rx_header_p, can2_rxd_data_buffer); ///< 解析摩擦轮数据
        switch (robot_control_data_pt->mode.fric_cover_mode) ///< 选择摩擦轮模式： 0关闭，1自适应，2高速，3低速
        {
        case fric_cover_off_mode_ENUM: ///< 0

        {
            fric_speed = 0;
            is_ok_fire = 0;

            break;
        }

        case fric_adaptive_speed_mode_ENUM: ///< 1

        {
            fric_speed = 8000;
            is_ok_fire = 1;
            break;
        }

        case fric_high_speed_mode_ENUM: ///< 2

        {
            fric_speed = 14000;
            is_ok_fire = 1;
            break;
        }

        case cover_on_ENUM: ///< 3

        {
            fric_speed = 16000;
            is_ok_fire = 1;

            break;
        }

        default:
        {
            break;
        }
        }
        ramp_calc(&shoot_control.fric1_ramp, fric_speed);
        ramp_calc(&shoot_control.fric1_ramp, fric_speed);
        shoot_control.fric_send_speed1 = (uint16_t)(shoot_control.fric1_ramp.out);
        shoot_control.fric_send_speed2 = (uint16_t)(shoot_control.fric2_ramp.out);
        if (Updata_Wave_Ch_Value(&last_wave_ch_value, &this_wave_ch_value))
        {
            osSignalSet(waveWheelTaskHandle, fire_one_bullet);
        }

        Set_Friction_Motor_Speed(-fric_speed, fric_speed, friction_motor_feedback_data); ///< 摩擦轮速度PID计算以及设置摩擦轮速度
        // float motor_date1 = 0;
        // float motor_date2 = 0;
        // motor_date1 = shoot_control.fric_send_speed1;
        // motor_date2 = friction_motor_feedback_data[0].speed_rpm;
        //Console.print("fric_speed:%0.2f,%0.2f\r\n", fric_speed, friction_motor_feedback_data[0].speed_rpm);
        osDelay(10);
    }
}

/**
*@brief 封装解析摩擦轮数据，中断中使用
 */
void Parse_Friction__Data_ISR(void)
{
    Parse_Friction_Wave_Motor_Feedback_Data(can2_rx_header_p, can2_rxd_data_buffer); ///< 解析摩擦轮数据
}
/**
  * @brief          射击初始化，初始化PID，遥控器指针，电机指针
  * @param[in]      void
  * @retval         返回空
  */
void shoot_init(void)
{

    // static const fp32 Trigger_speed_pid[3] = {TRIGGER_ANGLE_PID_KP, TRIGGER_ANGLE_PID_KI, TRIGGER_ANGLE_PID_KD};
    // shoot_control.shoot_mode = SHOOT_STOP;
    //遥控器指针
    // shoot_control.shoot_rc = get_remote_control_point();
    //电机指针
    // shoot_control.shoot_motor_measure = get_trigger_motor_measure_point();
    //初始化PID
    // PID_init(&shoot_control.trigger_motor_pid, PID_POSITION, Trigger_speed_pid, TRIGGER_READY_PID_MAX_OUT, TRIGGER_READY_PID_MAX_IOUT);
    //更新数据
    // shoot_feedback_update();
    /* 斜坡函数初始化 */
    ramp_init(&shoot_control.fric1_ramp, RAMP_STEP, 0);
    ramp_init(&shoot_control.fric2_ramp, RAMP_STEP, 0);
    shoot_control.fric_send_speed1 = 0;
    shoot_control.fric_send_speed2 = 0;
}
/**
 * @brief   射击任务初始化，主要是初始化获取数据
  */
void shooter_init(void)
{
    rc_data_pt = Get_Rc_Parsed_RemoteData_Pointer();
    robot_control_data_pt = Get_Parsed_RobotMode_Pointer();
    can2_rxd_data_buffer = Get_CAN2_Rxd_Buffer();
    can2_rx_header_p = Get_CAN2_Rx_Header();
    wave_motor_feedback_data = Get_Wave_Motor_Paresed_Data();

    erroe_integral = Get_Error_Integral();
    last_machine_angle = Get_Last_Machine_Angle();
    this_machine_angle = Get_This_Machine_Angle();
    judge_data = Get_Referee_Data();
}
/**
 * @brief                           解析云台两个摩擦轮电机和波轮电机的数据
 * @param CAN_RxHeaderTypeDef       can2 接受结构体头指针
 * @param uint8_t*                 can2 接受到的数据
 * @retval                          void
 */
void Parse_Friction_Wave_Motor_Feedback_Data(CAN_RxHeaderTypeDef *can_rx_header, uint8_t *can2_rxd_data_buffer)
{
    static uint8_t i = 0;
    switch (can_rx_header->StdId)
    {
    case CAN_3508_FRIC_ID_LEFT:
    case CAN_3508_FRIC_ID_RIGHT:
    {
        i = can_rx_header->StdId - CAN_SHOOTER_ALL_ID - 1;
        Calculate_Motor_Data(&friction_motor_feedback_data[i], can2_rxd_data_buffer);
        break;
    }

    default:
        break;
    }
}

/**
 *@brief    判断发射按钮是否被拨动
 *@retval   1:是    2：否
 *   */
int8_t Updata_Wave_Ch_Value(int16_t *last_wave_ch_value, int16_t *this_wave_ch_value)
{
    *last_wave_ch_value = *this_wave_ch_value;
    *this_wave_ch_value = rc_data_pt->rc.ch4;

    if ((*this_wave_ch_value == 660) && (*last_wave_ch_value != 660))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int8_t *Get_Is_OK_Fire(void)
{
    return &is_ok_fire;
}

const int32_t *Get_Fire_One_Bullet_Signal(void)
{
    return &fire_one_bullet;
}

const int32_t *Get_Fire_Five_Bullet_Signal(void)
{
    return &fire_five_bullet;
}

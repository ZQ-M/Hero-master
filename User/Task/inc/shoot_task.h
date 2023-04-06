#ifndef __SHOOT_TASK_H_
#define __SHOOT_TASK_H_

#include "cmsis_os.h"
#include "stm32f4xx.h"
#include "remote_task.h"
#include "can2_device.h"
#include "stdlib.h"
#include "console_print.h"
#include "referee_system.h"
#define RAMP_STEP 80
typedef struct
{
    // shoot_mode_e shoot_mode;
    // const RC_ctrl_t *shoot_rc;
    // const motor_measure_t *shoot_motor_measure;
    ramp_function_source_t fric1_ramp;
    uint16_t fric_send_speed1;
    ramp_function_source_t fric2_ramp;
    uint16_t fric_send_speed2;
    // pid_type_def trigger_motor_pid;
    // fp32 trigger_speed_set;
    // fp32 speed;
    // fp32 speed_set;
    // fp32 angle;
    // fp32 set_angle;
    // int16_t given_current;
    // int8_t ecd_count;

    // bool_t press_l;
    // bool_t press_r;
    // bool_t last_press_l;
    // bool_t last_press_r;
    // uint16_t press_l_time;
    // uint16_t press_r_time;
    // uint16_t rc_s_time;

    // uint16_t block_time;
    // uint16_t reverse_time;


    uint16_t heat_limit;
    uint16_t heat;
} shoot_control_t;

void Parse_Friction_Wave_Motor_Feedback_Data(CAN_RxHeaderTypeDef *p_can_rx_header,uint8_t *can2_rxd_data_buffer);
int8_t Updata_Wave_Ch_Value(int16_t *last_wave_ch_value,int16_t *this_wave_ch_value);
void Emission_Once_Time(void);
int8_t *Get_Is_OK_Fire(void);
const int32_t* Get_Fire_One_Bullet_Signal(void);
const int32_t* Get_Fire_Five_Bullet_Signal(void);
void Parse_Friction__Data_ISR(void);
#endif //__SHOOT_TASK_H_

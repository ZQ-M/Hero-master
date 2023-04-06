/**
 * @file remote_task.c
 * @brief 遥控器数据解析任务，解析出遥控器数据，并根据遥控器数据设置机器人当前模式配置的相关参数
 * @version 0.1
 * @date 2021-9-27
 * @copyright Copyright (c) 2021
 */

#include "remote_task.h"

static Rc_Ctrl_t remote_controller;                    ///< 本次解析的遥控器数据
static Rc_Ctrl_t last_time_remote_controller;          ///< 上次解析的遥控器数据
static Robot_control_data_t robot_control_data;        ///< 机器人模式配置数据
static uint16_t *sbus_rxd_len;                         ///< 本次遥控器接收到的数据长度
static uint8_t *rc_rx_buffer[2];                       ///< 遥控器两次接收数据缓冲
static const uint32_t remote_get_data_signal = 0x0001; ///< 遥控器接收数据信号
static const uint32_t remote_data_overtime = 50;       ///< 遥控器接收数据包超时时间
extern osThreadId remoteTaskHandle;                    ///< 遥控器任务句柄

void StartRemoteTask(void const *argument)
{
    static uint8_t rx_available_buffer_index;
    osEvent remote_get_data_event;

    rc_rx_buffer[0] = Get_Usart1_DMA_RxBuffer_One();
    rc_rx_buffer[1] = Get_Usart1_DMA_RxBuffer_Two();
    sbus_rxd_len = Get_Usart1_DMA_Rxd_DataLen();

    Usart1_RxDMA_Init();
    Rc_Data_Reset(&remote_controller);           ///< 本次数据重置
    Rc_Data_Reset(&last_time_remote_controller); ///< 上次数据重置
    Control_Data_Init(&robot_control_data);      ///< 初始化时的机器人模式设置

    osDelay(100);

    for (;;)
    {
        remote_get_data_event = osSignalWait(remote_get_data_signal, remote_data_overtime);
        //利用超时时间做不同的任务处理
        if (remote_get_data_event.status == osEventSignal)
        {
            if (remote_get_data_event.value.signals == remote_get_data_signal)
            {
                rx_available_buffer_index = Get_Rc_Available_Bufferx();

                if (Rc_Data_Check_Parse(rc_rx_buffer[rx_available_buffer_index], &remote_controller, *sbus_rxd_len)) ///<解析数据并检验
                {
                    Parse_Robot_Control_Data(&remote_controller, &last_time_remote_controller, &robot_control_data); ///< 解析出遥控器模式

                    Rc_Data_Copy(&last_time_remote_controller, &remote_controller); ///< 保存本次遥控器状态

                    Module_Reload(remote_control); ///< 更新遥控器状态用于离线判断
                }
                else
                {
                    Usart1_Rx_DMA_Reset();                                          ///< DMA重置
                    Rc_Data_Copy(&remote_controller, &last_time_remote_controller); ///< 校验出错保持遥控器数据不变
                }
            }
        }
        else if (remote_get_data_event.status == osEventTimeout)
        {
            Rc_Data_Reset(&remote_controller);

            Rc_Data_Reset(&last_time_remote_controller);
        }
    }
}

/**
 * @brief           通知 remoteTask 进行遥控器数据的解析，在 USART1 的串口中断中调用
 * @param[in]       none
 * @retval          void
 */

void Info_RemoteTask_Parse_Data(void)
{
    osSignalSet(remoteTaskHandle, remote_get_data_signal);
}

/**
 * @brief                       获取解析后的遥控器数据
 * @param[in]                   void
 * @retval {Rc_Ctrl_t*}         解析后的遥控器数据结构体指针
 */
Rc_Ctrl_t *Get_Rc_Parsed_RemoteData_Pointer(void)
{
    return &remote_controller;
}

/**
 * @brief                       获取解析后的机器人模式结构体指针
 * @param[in]                   void
 * @retval {Rc_Ctrl_t*}         解析后的解析后的机器人模式结构体指针
 */
Robot_control_data_t *Get_Parsed_RobotMode_Pointer(void)
{
    return &robot_control_data;
}

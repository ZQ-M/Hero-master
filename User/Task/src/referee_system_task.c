// #include "referee_system_task.h"
// #include "referee_system.h"

// static uint8_t *referee_system_rx_buffer[2];
// static uint16_t *referee_system_rxd_len;                       ///<裁判系统接收数据长度
// static const uint32_t referee_system_get_data_signal = 0x0001; ///< 裁判系统接受到的任务信号
// static uint8_t referee_system_rx_buffer_index;
// // static const uint32_t referee_system_data_overtime = 50;//使用了直接任务通知，暂时不需要
// extern osThreadId refereeSystemTaHandle;

// // static const uint8_t *judge_buf;
// static uint8_t judge_buf_copy[128];
// static uint32_t judge_buf_len = 0;
// void Referee_Init(void);                                                       ///<裁判系统初始化函数声明
// extern uint8_t Parse_Refere_System_Data(uint8_t *get_data, uint16_t data_len); ///< bug_log：声明始终被识别为定义
// const Judge_data_t *juder_date_after_parse;                                    ///< 解析后的裁判系统数据指针
// /**
//  * @brief    裁判系统调试宏定义,1为打开
//  * @note     后续添加config.h中
//  */
// #if 0
// #define DEBUG_LOG debug_log
// #define DEBUG_ERROR debug_error
// #else
// #define DEBUG_LOG(format, arg) /* DEBUG OFF */
// #define DEBUG_ERROR(err)       /* DEBUG OFF */
// #endif
// /*函数声明*/
// // uint8_t Parse Referee System Data(uint8_t *get_data, uint16_t data_len);///<裁判系统串口解析函数声明

// void StartRefereeSystemTask(void const *argument)
// {
//     // osEvent referee_system_get_data_event;
//     // Referee_Init();
//     Usart6_RxDMA_Init();

//     referee_system_rx_buffer[0] = Get_Usart6_DMA_RxBuffer_One();
//     referee_system_rx_buffer[1] = Get_Usart6_DMA_RxBuffer_Two();
//     referee_system_rxd_len = Get_Usart6_DMA_Rxd_DataLen();
//     juder_date_after_parse = Get_Referee_Data();
//     referee_system_rx_buffer_index = Get_USART6_DMA_Available_Bufferx(); ///< 可用的DMA区域
//     /*调试区域*/
//     // (void)referee_system_rxd_len; ///<避免warning
//     /*调试区域结束*/
//     osDelay(10);
//     for (;;)
//     {
//         // referee_system_get_data_event = osSignalWait(referee_system_get_data_signal, referee_system_data_overtime);
//         xTaskNotifyWait(0x00, 0xFFFFFFFF, &judge_buf_len, portMAX_DELAY);
//         //   xTaskNotifyWait(0x00,0xffffffff, &judge_buf_len,0xffff);
//         // if (referee_system_get_data_event.status == osEventSignal)
//         // {
//         // if (referee_system_get_data_event.value.signals == referee_system_get_data_signal)
//         // {
//         // referee_system_rx_buffer_index = Get_USART6_DMA_Available_Bufferx();

//         if (1) ///<解析数据并检验
//         /* debug_printf需定义DEBUG才能使用 */
//         {
//             LL_USART_TransmitData8(USART6, 9);
//             /*将接受的的原始数据拷贝为副本*/
//             memcpy(judge_buf_copy, referee_system_rx_buffer[referee_system_rx_buffer_index], judge_buf_len);
//             // debug_showdata1("judge_buf_len", judge_buf_len);
//             // for (uint8_t i = 0; i < judge_buf_len; i++)
//             // {
//             //     debug_print("%d ", judge_buf_copy[i]);
//             // }
//             // debug_print("\r\n");
//             Module_Reload(judge_system);                             ///< 更新裁判系统状态
//             // Parse_Refere_System_Data(judge_buf_copy, judge_buf_len); ///<解析裁判系统数据

//             // Console.print("%0.2f\r\n", juder_date_after_parse ->power_heat_data.shooter_id1_17mm_cooling_heat);
//             // Console.print("%0.2f,0.2f\r\n", referee_system_rxd_len, juder_date_after_parse);
//         }
//         else
//         {
//             Usart6_Rx_DMA_Reset();
//         }
//         Usart6_Rx_DMA_Reset();
//         // }
//         // }
//         // else if (referee_system_get_data_event.status == osEventTimeout)
//         // {
//         // }

//         osDelay(10);
//     }
// }
// /**
//  * @brief       裁判系统初始化
//  * @param[in]   void
//  * @return      void
//  */
// void Referee_Init(void)
// {
//     Usart6_RxDMA_Init();

//     referee_system_rx_buffer[0] = Get_Usart6_DMA_RxBuffer_One();
//     referee_system_rx_buffer[1] = Get_Usart6_DMA_RxBuffer_Two();
//     referee_system_rxd_len = Get_Usart6_DMA_Rxd_DataLen();
//     juder_date_after_parse = Get_Referee_Data();
// }

// /**
//  * @brief           通知裁判系统任务进行解析，在串口 6 的中断函数中进行调用
//  * @param[in]       none
//  * @retval          void
//  */
// void Info_Referee_System_Task_Parse_Data(void)
// {
//     osSignalSet(refereeSystemTaHandle, referee_system_get_data_signal);
// }
// /**
//  * @brief   通知裁判系统任务进行解析
//  */
// void Inform_Referee_System_Task_With_len(uint32_t send_date)
// {
//     /* 向任务发送通知，参数是任务句柄 */
//     xTaskNotifyFromISR(refereeSystemTaHandle, send_date, eSetValueWithOverwrite, NULL);
// }

#include "reference_analysis_task.h"

extern osThreadId ReferenceTaskHandle;

static const int32_t reference_task_get_date_signal = 0x000000ff;
static uint8_t uart8_rx_available_buffer_index; //< 返回双缓冲中可读取的区域[0&1，作为数组下标]
static uint8_t *reference_system_rxd_buffer[2]; //< 裁判系统双缓冲接收数组
static uint16_t *rxd_data_len;                  //< 裁判系统接收数据长度
static uint8_t date_copy[128];                  ///< 复制收到的数据
const Judge_data_t *juder_date_finish;          ///< 暂时存储解析后的裁判系统数据，调试使用
void StartReferenceTaskTask(void const *argument)
{
    osEvent reference_get_data_event;                              ///< 通知解析
    Uart8_Rx_Dma_Init();                                           ///< DMA初始化
    rxd_data_len = Get_Uart8_DMA_Rxd_DataLen();                    ///< 指向接收长度（解析使用）
    reference_system_rxd_buffer[0] = Get_Uart8_DMA_RxBuffer_One(); ///< 指向双缓冲区域
    reference_system_rxd_buffer[1] = Get_Uart8_DMA_RxBuffer_Two(); ///< 指向双缓冲区域
    juder_date_finish = Get_Referee_Data();                        //< 指向解析后的数据
    osDelay(100);
    for (;;)
    {
        reference_get_data_event = osSignalWait(reference_task_get_date_signal, osWaitForever); ///< 等待来自中断的通知信号
        if (reference_get_data_event.status == osEventSignal)
        {
            if (reference_get_data_event.value.signals == reference_task_get_date_signal) ///< 收到信号，开始解析
            {
                uart8_rx_available_buffer_index = Get_Reference_Available_Bufferx();
                memcpy(date_copy, reference_system_rxd_buffer[uart8_rx_available_buffer_index], *rxd_data_len); ///< 拷贝原始数据

                debug_showdata1("judge_buf_len", *rxd_data_len); ///< 打印数据长度
                                                                 /**
                 * @brief   打印原始数据
                 */
                for (uint8_t i = 0; i < *rxd_data_len; i++)
                {
                    debug_print("%d ", date_copy[i]);
                }
                debug_print("\r\n");

                Parse_Refere_System_Data(date_copy, *rxd_data_len); ///<解析裁判系统数据
                Module_Reload(judge_system);                        ///< 裁判系统离线检测
            }
        }
        else if (reference_get_data_event.status == osEventTimeout)
        {
            // TODO: 未按时接收到裁判系统数据，进行相应的处理，比如说离线处理
        }
        osDelay(200);
    }
}
/**
 * @brief       通知裁判系统解析任务开始解析
 * @note        在uart8中断函数中调用
 */
void Inform_Reference_Analysis_Task_Pasre_Data(void)
{

    osSignalSet(ReferenceTaskHandle, reference_task_get_date_signal);
}

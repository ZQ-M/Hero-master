#include "referee_system_task.h"

static uint8_t *referee_system_rx_buffer[2];
static uint16_t *referee_system_rxd_len;
static const uint32_t referee_system_get_data_signal = 0x0001; ///< 裁判系统接受到的任务信号
static uint8_t referee_system_rx_buffer_index;
static const uint32_t referee_system_data_overtime = 50;  
extern osThreadId refereeSystemTaHandle;   

static const uint8_t *judge_buf;
static uint8_t judge_buf_copy[128];
static uint32_t judge_buf_len = 0;

/* 裁判系统调试宏定义 
后续添加system_device.c中
*/
#if 0
	#define JUDGE_LOG DEBUG_LOG
	#define JUDGE_ERROR DEBUG_ERROR
#else
	#define JUDGE_LOG(format, arg...) /* DEBUG OFF */
	#define JUDGE_ERROR(err) /* DEBUG OFF */
#endif
/*函数声明*/
//uint8_t Parse Referee System Data(u8 *get_data, u16 data_len);///<裁判系统串口解析函数声明


void StartRefereeSystemTask(void const *argument)
{
    osEvent referee_system_get_data_event;
    Usart6_RxDMA_Init();

    referee_system_rx_buffer[0] = Get_Usart6_DMA_RxBuffer_One();
    referee_system_rx_buffer[1] = Get_Usart6_DMA_RxBuffer_Two();
    referee_system_rxd_len = Get_Usart6_DMA_Rxd_DataLen();
    /*调试区域*/
    (void)referee_system_rxd_len;///<避免warning

    /*调试区域结束*/
    osDelay(10);

    for (;;)
    {
        referee_system_get_data_event = osSignalWait(referee_system_get_data_signal, referee_system_data_overtime);

        if (referee_system_get_data_event.status == osEventSignal)
        {
            if (referee_system_get_data_event.value.signals == referee_system_get_data_signal)
            {
                referee_system_rx_buffer_index = Get_USART6_DMA_Available_Bufferx();

                if (1)///<解析数据并检验
                {
                    /*输出接收到的裁判系统数据*/
                    Console.print("%s\r\n", referee_system_rx_buffer[referee_system_rx_buffer_index]);

                    LL_USART_TransmitData8(USART6, 9);
                    /*将接受的的原始数据拷贝为副本*/
		            memcpy(judge_buf_copy, judge_buf, judge_buf_len);
                    
                    Module_Reload(judge_sy stem); ///< 更新裁判系统状态

                   Parse_Refere_System_Data(judge_buf_copy, judge_buf_len);
                }
                else
                {
                    Usart6_Rx_DMA_Reset();
                }
            }
        }
        else if (referee_system_get_data_event.status == osEventTimeout)
        {
        }

        osDelay(10);
    }
}
/**
 * @brief           通知裁判系统任务进行解析，在串口 6 的中断函数中进行调用
 * @param[in]       none
 * @retval          void
 */
void Info_Referee_System_Task_Parse_Data(void)
{
    osSignalSet(refereeSystemTaHandle, referee_system_get_data_signal);
}

/*
  函数名：Analysis_Judge_System
  描述  ：解析裁判系统数据
  参数  ：get_data需要解析的帧头数据，data_len数据长度
  返回值：0--解析失败 1--解析成功
*/
// uint8_t Parse Referee System Data(u8 *get_data, u16 data_len)
// {
// 	u8 a5_position[8]; //0xA5的位置
// 	u8 a5_number = 0;  //0xA5的个数（数据包个数）
// 	u16 data_length[8];  //每个data数据包的长度
	
// 	//寻找帧头
// 	Find_All_A5(get_data, data_len, a5_position, &a5_number);
	
// 	//解析所有数据包
// 	for(u8 i=0; i<a5_number; i++)
// 	{
// 		//解析帧头数据
// 		if( Analysis_Frame_Header(&get_data[ (a5_position[i]) ], &data_length[i], NULL) == 0)
// 		{
// 			JUDGE_LOG("Analysis No.%d frame header error.", i);
// 			continue;
// 		}
		
// 		//整包CRC16校验
// 		if( Check_Package_Crc16(&get_data[ (a5_position[i]) ], (data_length[i]+9)) == 0 )
// 		{
// 			JUDGE_LOG("CRC16 check No.%d fail.", i);
// 			continue;
// 		}
		
// 		//解析该数据包
// 		if( Analysis_Data(&get_data[ (a5_position[i]) ], data_length[i]) == 0)
// 		{
// 			JUDGE_LOG("Analysis No.%d data fail.", i);
// 			continue;
// 		}
		
// 		// DEBUG_PRINT("x%d len:%d p:%d id:%d\r\n", i, data_length[i], a5_position[i], Analysis_Cmd_Id( &get_data[ (a5_position[i]) ] ) );
		
		
// 	}
	
// 	return 1;
// }


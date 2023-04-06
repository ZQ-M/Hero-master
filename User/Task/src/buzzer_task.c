/**
 * @file buzzer_task.c
 * @brief 蜂鸣器任务，使用消息队列，但是传入的参数是结构体指针，会出现覆盖情况，待改进
 * @version 0.1
 * @date 2021-10-03
 * @copyright Copyright (c) 2021
 */

#include "buzzer_task.h"

extern osMessageQId buzzerQueueHandle; ///< 蜂鸣器数据队列
extern osThreadId buzzerTaskHandle;    ///< 蜂鸣器任务句柄
static Beep_Play_t *beep_play;         ///< 蜂鸣器数据结构体指针

void StartBuzzerTask(void const *argument)
{
    osEvent buzzer_task_get_data_event;

    Buzzer_Init();

    osDelay(100);

    for (;;)
    {
        buzzer_task_get_data_event = osMessageGet(buzzerQueueHandle, osWaitForever);

        if (buzzer_task_get_data_event.status == osEventMessage)
        {
            beep_play = (Beep_Play_t *)buzzer_task_get_data_event.value.v;
            for (uint8_t i = 0; i < beep_play->beep_time; i++)
            {
                BUZZER_ON(beep_play->pulse);
                osDelay(beep_play->delay_time);

                BUZZER_OFF();
                osDelay(beep_play->delay_time);
            }
        }
    }
}

/**
 * @brief               蜂鸣器播报数据
 * @param beep_time     播报次数
 * @param pulse         占空比
 * @param delay_time    响应时间
 * @return osStatus     发送状态
 */
osStatus Set_Beep_Time(uint8_t beep_time, uint16_t pulse, uint16_t delay_time)
{
    static Beep_Play_t beep_msg;
    beep_msg.beep_time = beep_time;
    beep_msg.pulse = pulse;
    beep_msg.delay_time = delay_time;
    return osMessagePut(buzzerQueueHandle, (uint32_t)&beep_msg, 0);
}

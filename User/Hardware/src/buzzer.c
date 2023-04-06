#include "buzzer.h"
#include "cmsis_os.h"
/**
 * @brief           蜂鸣器初始化
 * @param[in]       void
 * @retval          void
 */
void Buzzer_Init(void)
{
    LL_TIM_CC_EnableChannel(TIM12,LL_TIM_CHANNEL_CH1);
    LL_TIM_EnableAllOutputs(TIM12);
    LL_TIM_EnableCounter(TIM12);
    LL_TIM_OC_SetCompareCH1(TIM12, 0);
}
/**
 * @brief   硬件初始化时的蜂鸣
 * @author  whale
  */
void Buzzer_Bsp_Init(void)
{
    BUZZER_ON(NOTE_DH4);
    osDelay(200);
	BUZZER_OFF();
	BUZZER_ON(NOTE_DH2);
    osDelay(200);
	BUZZER_OFF();
}
/**
 * @brief   播放一段音乐
 * @author  whale
  */
void Music_Play(uint8_t flag)
{
if(flag==1)           
    {
            BUZZER_ON(NOTE_D1);
                osDelay(300);
                BUZZER_ON(NOTE_D1);
                osDelay(300);
                BUZZER_ON(NOTE_D5);
                osDelay(300);
                BUZZER_ON(NOTE_D5);
                osDelay(300);
                BUZZER_ON(NOTE_D6);
                osDelay(300);
                BUZZER_ON(NOTE_D6);
                osDelay(300);
                BUZZER_ON(NOTE_D5);
                osDelay(300);
                BUZZER_ON(NOTE_D4);
                osDelay(300);
                BUZZER_ON(NOTE_D4);
                osDelay(300);
                BUZZER_ON(NOTE_D3);
                osDelay(300);
                BUZZER_ON(NOTE_D3);
                osDelay(150);
                BUZZER_ON(NOTE_D3);
                osDelay(150);
                BUZZER_ON(NOTE_D2);
                osDelay(300);
                BUZZER_ON(NOTE_D2);
                osDelay(300);

                BUZZER_ON(NOTE_D1);
                osDelay(300);
                BUZZER_ON(NOTE_D1);
                osDelay(300);
                BUZZER_OFF();
    }
}


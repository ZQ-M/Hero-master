#ifndef __BUZZER_H_
#define __BUZZER_H_

#include "tim.h"

#define BUZZER_ON(pulse)                           \
    {                                              \
        TIM12->ARR = pulse;                        \
        LL_TIM_OC_SetCompareCH1(TIM12, pulse / 2); \
    }
#define BUZZER_OFF() LL_TIM_OC_SetCompareCH1(TIM12, 0);

/* 添加了音调，后续实现不同状态的蜂鸣器声音 */

void Buzzer_Init(void);
void Music_Play(uint8_t);   ///< 一段音乐
void Buzzer_Bsp_Init(void); ///< 硬件初始化之后的蜂鸣器音乐
///< 出错时的蜂鸣器
///< 调试蜂鸣器等

/* 音调定义 */
#define D0 1
#define D1 262
#define D2 293
#define D3 329
#define D4 349
#define D5 392
#define D6 440
#define D7 494

#define M1 523
#define M2 586
#define M3 658
#define M4 697
#define M5 783
#define M6 879
#define M7 987

#define H1 1045
#define H2 1171
#define H3 1316
#define H4 1393
#define H5 1563
#define H6 1755
#define H7 1971

#define NOTE_D0 1
#define NOTE_D1 294
#define NOTE_D2 330
#define NOTE_D3 370
#define NOTE_D4 393
#define NOTE_D5 441
#define NOTE_D6 495
#define NOTE_D7 556

#define NOTE_DL1 147
#define NOTE_DL2 165
#define NOTE_DL3 175
#define NOTE_DL4 196
#define NOTE_DL5 221
#define NOTE_DL6 248
#define NOTE_DL7 278

#define NOTE_DH1 589
#define NOTE_DH2 661
#define NOTE_DH3 700
#define NOTE_DH4 786
#define NOTE_DH5 882
#define NOTE_DH6 990
#define NOTE_DH7 112

#define WHOLE 1
#define HALF 0.5
#define QUARTER 0.25
#define EIGHTH 0.25
#define SIXTEENTH 0.625

#endif //__BUZZER_H_

#ifndef __LED_H_
#define __LED_H_

#include "stm32f4xx_ll_gpio.h"
#include "cmsis_os.h"

#define LED_RED_ON()     GPIOE->BSRR = LL_GPIO_PIN_11 << 16
#define LED_RED_OFF()    GPIOE->BSRR = LL_GPIO_PIN_11
#define LED_RED_TOGGLE() GPIOE->ODR ^= LL_GPIO_PIN_11

#define Laser_ON()       GPIOG->BSRR = LL_GPIO_PIN_13///<打开激光

#define LED_GREEN_ON()     GPIOF->BSRR = LL_GPIO_PIN_14 << 16
#define LED_GREEN_OFF()    GPIOF->BSRR = LL_GPIO_PIN_14
#define LED_GREEN_TOGGLE() GPIOF->ODR ^= LL_GPIO_PIN_14

#define LED_FLOW_ON(led_num)     LL_GPIO_ResetOutputPin(GPIOG, LL_GPIO_PIN_8 >> led_num)///<移位实现流水灯
#define LED_FLOW_OFF(led_num)    LL_GPIO_SetOutputPin(GPIOG, LL_GPIO_PIN_8 >> led_num)
#define LED_FLOW_TOGGLE(led_num) GPIOG->ODR ^= LL_GPIO_PIN_8 >> led_num

#define LED_FLOW_ON_PIN(LL_GPIO_PIN)     LL_GPIO_ResetOutputPin(GPIOG, LL_GPIO_PIN)
#define LED_FLOW_OFF_PIN(LL_GPIO_PIN)    LL_GPIO_SetOutputPin(GPIOG, LL_GPIO_PIN)
#define LED_FLOW_TOGGLE_PIN(LL_GPIO_PIN) GPIOG->ODR ^= LL_GPIO_PIN

void Led_Flow_On1(void);
void Led_Flow_Off1(void);
void Led_Flow_Flash(void);

#endif // __LED_H_

#include "led.h"

void Led_Flow_On1(void)
{
	for(int i=0; i<8; i++)
	{
		LED_FLOW_ON(i);
	}
}

void Led_Flow_Off1(void)
{
	for(uint8_t i=0; i<8; i++)
	{
		LED_FLOW_OFF(i);
	}
}

void Led_Flow_On2(void)
{
	for(int i=0; i<8; i++)
	{
		LED_FLOW_ON(i);
		osDelay(30);
	}
}

void Led_Flow_Off2(void)
{
	for(uint8_t i=0; i<8; i++)
	{
		LED_FLOW_OFF(i);
		osDelay(30);
	}
}

void Led_Flow_Flash(void)
{
    Led_Flow_Off1();
	osDelay(50);
    Led_Flow_On2();
    Led_Flow_Off2();
}

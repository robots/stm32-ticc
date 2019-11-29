#include "platform.h"

#include "exti.h"


exti_fnc_t exti_fnc[20];

uint8_t exti_irqchan[] = {
	EXTI0_1_IRQn, EXTI0_1_IRQn, EXTI2_3_IRQn, EXTI2_3_IRQn, EXTI4_15_IRQn,
	EXTI4_15_IRQn, EXTI4_15_IRQn, EXTI4_15_IRQn, EXTI4_15_IRQn, EXTI4_15_IRQn,
	EXTI4_15_IRQn, EXTI4_15_IRQn, EXTI4_15_IRQn, EXTI4_15_IRQn, EXTI4_15_IRQn,
	// below are internal events
	PVD_VDDIO2_IRQn, RTC_IRQn, 0,0,0
};

void exti_set_handler(uint8_t exti, exti_fnc_t fnc)
{
	if (exti >= 20)
		return;

	exti_fnc[exti] = fnc;
}

void exti_enable(uint8_t exti, EXTITrigger_TypeDef trig, uint8_t gpio)
{
	if (exti >= 20)
		return;

	EXTI_InitTypeDef EXTI_InitStructure = {
		.EXTI_Mode = EXTI_Mode_Interrupt,
		.EXTI_Trigger = trig,
		.EXTI_Line = 1 << exti,
		.EXTI_LineCmd = ENABLE,
	};

	NVIC_InitTypeDef EXT_Int = {
		.NVIC_IRQChannelPriority = 10,
		.NVIC_IRQChannelCmd = ENABLE,
		.NVIC_IRQChannel = exti_irqchan[exti],
	};

	if (exti < 16) {
		SYSCFG_EXTILineConfig(gpio, exti);
	}

	EXTI_Init(&EXTI_InitStructure);
	NVIC_Init(&EXT_Int);
}

void exti_disable(uint8_t exti)
{
	if (exti >= 20)
		return;


	EXTI->EMR &= 1 << exti;
/*
	NVIC_InitTypeDef EXT_Int = {
		.NVIC_IRQChannelPreemptionPriority = 14,
		.NVIC_IRQChannelSubPriority = 0,
		.NVIC_IRQChannelCmd = DISABLE,
		.NVIC_IRQChannel = exti_irqchan[exti],
	};

	NVIC_Init(&EXT_Int);*/

}

void exti_trigger(uint8_t exti)
{
	if (exti >= 20)
		return;

	EXTI->SWIER |= 1 << (exti);
}

#define EXEC(x) \
	do { \
		if (EXTI->PR & EXTI->IMR & (1 << (x))) { \
			if (exti_fnc[(x)]) \
				exti_fnc[(x)]();\
			EXTI->PR = 1 << (x); \
		} \
	} while (0);

void EXTI0_1_IRQHandler(void)
{
	EXEC(0);
	EXEC(1);
}

void EXTI2_3_IRQHandler(void)
{
	EXEC(2);
	EXEC(3);
}

void EXTI4_15_IRQHandler(void)
{
	EXEC(4);
	EXEC(5);
	EXEC(6);
	EXEC(7);
	EXEC(8);
	EXEC(9);
	EXEC(10);
	EXEC(11);
	EXEC(12);
	EXEC(13);
	EXEC(14);
	EXEC(15);
}

void PVD_VDDIO2_IRQHandler(void)
{
	EXEC(16);
}

void RTC_IRQHandler(void)
{
	EXEC(17);
}


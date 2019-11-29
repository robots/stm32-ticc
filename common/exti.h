#ifndef EXTI_h_
#define EXTI_h_

#include "platform.h"

typedef void(*exti_fnc_t)(void);

enum exti_interrupt_t {
	EXTI_0,
	EXTI_1,
	EXTI_2,
	EXTI_3,
	EXTI_4,
	EXTI_5,
	EXTI_6,
	EXTI_7,
	EXTI_8,
	EXTI_9,
	EXTI_10,
	EXTI_11,
	EXTI_12,
	EXTI_13,
	EXTI_14,
	EXTI_15,
	EXTI_16,
	EXTI_17,
	EXTI_18,
	EXTI_19,
};

void exti_set_handler(uint8_t exti, exti_fnc_t fnc);
void exti_enable(uint8_t exti, EXTITrigger_TypeDef trig, uint8_t gpio);
void exti_disable(uint8_t exti);
void exti_trigger(uint8_t exti);


#endif

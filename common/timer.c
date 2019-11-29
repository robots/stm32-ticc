#include "platform.h"

#include <string.h>

#include "timer.h"

timer_fnc_t timer_handler[] = {NULL, NULL, NULL, NULL, NULL};
const uint8_t timer_irqc[] = {TIM2_IRQn, TIM3_IRQn, TIM14_IRQn, TIM16_IRQn, TIM17_IRQn};
const uint32_t timer_rcc[] = {RCC_APB1Periph_TIM2, RCC_APB1Periph_TIM3, RCC_APB1Periph_TIM14, RCC_APB2Periph_TIM16, RCC_APB2Periph_TIM17  };
const TIM_TypeDef *timer_tim[] = {TIM2, TIM3, TIM14, TIM16, TIM17};
uint32_t timer_timeouts[] = {0, 0, 0, 0, 0};

void timer_init(uint8_t timer)
{
	TIM_TypeDef *timx = timer_tim[timer];

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	NVIC_InitTypeDef NVIC_InitStructure = {
		.NVIC_IRQChannel = timer_irqc[timer],
		.NVIC_IRQChannelPriority = 10,
		.NVIC_IRQChannelCmd = DISABLE,
	};

	timer_timeouts[timer] = 0;
	timer_handler[timer] = NULL;

	uint32_t prescaler = 48;

	RCC->APB1ENR |= timer_rcc[timer];

	TIM_DeInit(timx);

	TIM_TimeBaseStructure.TIM_Period = 10000; // dummy value
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(timx, &TIM_TimeBaseStructure);

	timx->CR1 |= TIM_CR1_URS;

	TIM_ITConfig(timx, TIM_IT_Update, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void timer_delay(uint8_t timer, uint32_t microsec)
{
	TIM_TypeDef *timx = timer_tim[timer];

	timer_timeout(timer, microsec);
	while (timx->CR1 & TIM_CR1_CEN);

	timx->CR1 &= ~TIM_CR1_CEN;
}

void timer_set_handler(uint8_t timer, timer_fnc_t fnc)
{
	timer_handler[timer] = fnc;
}

void timer_timeout(uint8_t timer, uint32_t microsec)
{
	TIM_TypeDef *timx = timer_tim[timer];

	timx->CR1 &= ~TIM_CR1_CEN;

	if (timer == TIMER_TIM2) {
		timer_timeouts[timer] = 0;
		timx->ARR = microsec;
	} else {
		timer_timeouts[timer] = microsec >> 16;
		timx->ARR = microsec & 0xFFFF;
	}

	timx->CR1 |= TIM_CR1_CEN;
	timx->EGR |= TIM_EGR_UG;
}

void timer_abort(uint8_t timer)
{
	TIM_TypeDef *timx = timer_tim[timer];

	timx->CR1 &= ~TIM_CR1_CEN;
}

#define TIM_ISR(x) void TIM ## x ## _IRQHandler(void) \
{ \
	if (TIM_GetITStatus(TIM ## x, TIM_IT_Update)) { \
		TIM_ClearITPendingBit(TIM ## x, TIM_IT_Update); \
\
		if (timer_timeouts[TIMER_TIM ## x] == 0) { \
			TIM ## x->CR1 &= ~TIM_CR1_CEN; \
\
			if (timer_handler[TIMER_TIM ## x]) { \
				timer_handler[TIMER_TIM ## x](); \
			} \
		} else { \
			timer_timeouts[TIMER_TIM ## x]--; \
			TIM ## x->ARR = 0xffff; \
			TIM ## x->EGR |= TIM_EGR_UG; \
		} \
	} \
}

TIM_ISR(2);
TIM_ISR(3);
TIM_ISR(14);
TIM_ISR(16);
TIM_ISR(17);

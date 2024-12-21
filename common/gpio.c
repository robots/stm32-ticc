
#include "platform.h"

#include "gpio.h"


void GPIO_InitBatch(const struct gpio_init_table_t *t, int num)
{
	int i;

	uint32_t reg;
	for (i = 0; i < num; i++) {
		uint16_t pos = 0;

		GPIO_Set(&t[i], t[i].state);

		for (pos = 0; pos < 16; pos++) {
			if (!(t[i].pin & (1 << pos))) {
				continue;
			}

			if (t[i].af) {
				uint32_t idx = ((uint32_t)(pos & (uint32_t)0x07U) * 4U);
				reg = t[i].gpio->AFR[pos >> 3];
				reg &= ~((uint32_t)0xFU << idx) ;
				reg |= ((uint32_t)(t[i].af) << idx);
				t[i].gpio->AFR[pos >> 3] = reg;
			}

			if ((t[i].mode == GPIO_Mode_OUT) || (t[i].mode == GPIO_Mode_AF)) {
				reg = t[i].gpio->OSPEEDR;
				reg &= ~(3 << (pos * 2));
				reg |= (t[i].speed << (pos * 2));
				t[i].gpio->OSPEEDR = reg;

				reg = t[i].gpio->OTYPER;
				reg &= ~(1 << pos);
				reg |= (t[i].otype << pos);
				t[i].gpio->OTYPER = (uint16_t)reg;
			}

			reg = t[i].gpio->MODER;
			reg &= ~(3 << (pos * 2));
			reg |= (t[i].mode << (pos * 2));
			t[i].gpio->MODER = reg;

			reg = t[i].gpio->PUPDR;
			reg &= ~(3 << (pos * 2));
			reg |= (t[i].pupd << (pos * 2));
			t[i].gpio->PUPDR = reg;
		}
	}
}

uint8_t GPIO_WaitState(const struct gpio_init_table_t *gpio, uint8_t state)
{
	volatile uint32_t time = 0xFFFFFF;

	while (--time && ((!!(gpio->gpio->IDR & gpio->pin)) != (!!state)));

	return (time > 0)?0:1;
}

void GPIO_Set(const struct gpio_init_table_t *gpio, uint8_t state)
{
	switch (state) {
		case GPIO_SET:
#if defined(STM32F4XX)
			gpio->gpio->BSRRL = gpio->pin;
#else
			gpio->gpio->BSRR = gpio->pin;
#endif
			break;
		case GPIO_RESET:
#if defined(STM32F4XX)
			gpio->gpio->BSRRH = gpio->pin;
#else
			gpio->gpio->BRR = gpio->pin;
#endif
			break;
		default:
			break;
	}
}

uint8_t GPIO_Get(const struct gpio_init_table_t *gpio)
{
	return !!(gpio->gpio->IDR & gpio->pin);
}

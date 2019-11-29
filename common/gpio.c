
#include "platform.h"

#include "gpio.h"


void GPIO_InitBatch(const struct gpio_init_table_t *t, int num)
{
	GPIO_InitTypeDef gis;
	int i;

	for (i = 0; i < num; i++) {
		gis.GPIO_Pin = t[i].pin;
		gis.GPIO_Speed = t[i].speed;
		gis.GPIO_Mode = t[i].mode;
#if defined(STM32F4XX) || defined(STM32F0XX)
		gis.GPIO_OType = t[i].otype;
		gis.GPIO_PuPd = t[i].pupd;
#endif

		GPIO_Init(t[i].gpio, &gis);

		switch (t[i].state) {
			case GPIO_SET:
#if defined(STM32F4XX)
				t[i].gpio->BSRRH = t[i].pin;
#else
				t[i].gpio->BSRR = t[i].pin;
#endif
				break;
			case GPIO_RESET:
#if defined(STM32F4XX)
				t[i].gpio->BSRRL = t[i].pin;
#else
				t[i].gpio->BRR = t[i].pin;
#endif
				break;
			default:
				break;
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
			gpio->gpio->BSRRH = gpio->pin;
#else
			gpio->gpio->BSRR = gpio->pin;
#endif
			break;
		case GPIO_RESET:
#if defined(STM32F4XX)
			gpio->gpio->BSRRL = gpio->pin;
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

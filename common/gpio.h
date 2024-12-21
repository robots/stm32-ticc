#ifndef GPIO_h_
#define GPIO_h_

#include "platform.h"

enum gpio_state_t {
	GPIO_DEFAULT=0,
	GPIO_RESET,
	GPIO_SET,
};

struct gpio_init_table_t {
	GPIO_TypeDef* gpio;

	uint16_t pin;
	GPIOMode_TypeDef mode;
	GPIOSpeed_TypeDef speed;
	GPIOPuPd_TypeDef pupd;
	GPIOOType_TypeDef otype;
	uint16_t af;

	enum gpio_state_t state;
};

void GPIO_InitBatch(const struct gpio_init_table_t *t, int num);
uint8_t GPIO_WaitState(const struct gpio_init_table_t *gpio, uint8_t state);
void GPIO_Set(const struct gpio_init_table_t *gpio, uint8_t state);
uint8_t GPIO_Get(const struct gpio_init_table_t *gpio);

#endif

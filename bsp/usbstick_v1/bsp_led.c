
#include "platform.h"

#include "gpio.h"
#include "bsp_led.h"

const struct gpio_init_table_t led_gpio[] = {
	{
		.gpio = GPIOA,
		.pin = GPIO_Pin_15,
		.mode = GPIO_Mode_OUT,
		.otype = GPIO_OType_PP,
		.speed = GPIO_Speed_Level_3,
	},
	{
		.gpio = GPIOB,
		.pin = GPIO_Pin_3,
		.mode = GPIO_Mode_OUT,
		.otype = GPIO_OType_PP,
		.speed = GPIO_Speed_Level_3,
	},
};

const uint8_t led_pol[] = {
	1, 1,
};

const uint32_t led_gpio_cnt = ARRAY_SIZE(led_gpio);


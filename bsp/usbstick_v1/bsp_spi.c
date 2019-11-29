#include "platform.h"

#include "gpio.h"
#include "bsp_spi.h"

const struct gpio_init_table_t spi_gpio[] = {
	{ // SCK 
		.gpio = GPIOA,
		.pin = GPIO_Pin_5,
		.mode = GPIO_Mode_AF,
		.otype = GPIO_OType_PP,
		.speed = GPIO_Speed_Level_3,
	},
	{ // MOSI
		.gpio = GPIOA,
		.pin = GPIO_Pin_7,
		.mode = GPIO_Mode_AF,
		.otype = GPIO_OType_PP,
		.speed = GPIO_Speed_Level_3,
	},
	{ // MISO
		.gpio = GPIOA,
		.pin = GPIO_Pin_6,
		.mode = GPIO_Mode_AF,
		.otype = GPIO_OType_PP,
		.speed = GPIO_Speed_Level_3,
		.pupd  = GPIO_PuPd_NOPULL,
	},
};

const struct gpio_init_table_t spi_cs_gpio[] = {
	{ // CS0 
		.gpio = GPIOA,
		.pin = GPIO_Pin_3,
		.mode = GPIO_Mode_OUT,
		.speed = GPIO_Speed_Level_3,
		.otype = GPIO_OType_PP,
		.state = GPIO_SET,
	},
	{ // CS0 
		.gpio = GPIOA,
		.pin = GPIO_Pin_4,
		.mode = GPIO_Mode_OUT,
		.speed = GPIO_Speed_Level_3,
		.otype = GPIO_OType_PP,
		.state = GPIO_SET,
	},
};

const int spi_gpio_cnt = ARRAY_SIZE(spi_gpio);
const int spi_cs_gpio_cnt = ARRAY_SIZE(spi_cs_gpio);


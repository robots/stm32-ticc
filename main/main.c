#include "platform.h"

#include "usbd_cdc_core.h"
#include "usbd_usr.h"

#include "console.h"
#include "systime.h"
#include "led.h"

#include "ticc.h"

USB_CORE_HANDLE	USB_Device_dev;

int main(void)
{
	SystemInit();
	RCC->AHBENR |= RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB;
	RCC->APB2ENR |= RCC_APB2Periph_SYSCFG;

	console_init();
	systime_init();
	led_init();
	ticc_init();

	USBD_Init(&USB_Device_dev, &USR_desc, &USBD_CDC_cb, &USR_cb);
	
	while (1) {
		systime_periodic();
		led_periodic();
		ticc_periodic();
	}
} 

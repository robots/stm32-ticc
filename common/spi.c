#include "platform.h"
#include "gpio.h"

// just to satisfy NULL
#include <string.h>

#include "bsp_spi.h"
#include "spi.h"

volatile uint8_t spi_locked = 0;
volatile uint8_t spi_done = 1;
volatile uint8_t spi_csn = 0;
volatile uint8_t spi_dummy = 0;
volatile uint8_t spi_inited = 0;

static void spi_cs(uint8_t state);

enum {
	GPIO_SCK,
	GPIO_MOSI,
	GPIO_MISO,
};

void spi_init(void)
{
	SPI_InitTypeDef  SPIConf;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	if (spi_inited == 1)
		return;

	spi_inited = 1;

	RCC->APB2ENR |= RCC_APB2Periph_SPI1;
	RCC->AHBENR |= RCC_AHBPeriph_DMA1;

	// Reset SPI and DMA Channels
	SPI_I2S_DeInit(SPI1);
	DMA_DeInit(DMA1_Channel2);
	DMA_DeInit(DMA1_Channel3);

	// Configure SPI1 pins
	//GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
	GPIO_InitBatch(spi_gpio, spi_gpio_cnt);
	GPIO_InitBatch(spi_cs_gpio, spi_cs_gpio_cnt);

	SPIConf.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPIConf.SPI_Mode = SPI_Mode_Master;
	SPIConf.SPI_DataSize = SPI_DataSize_8b;
	SPIConf.SPI_CPOL = SPI_CPOL_Low;
	SPIConf.SPI_CPHA = SPI_CPHA_1Edge;
	SPIConf.SPI_NSS = SPI_NSS_Soft;
	SPIConf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // 48Mhz clock / 8 = 6Mhz
	SPIConf.SPI_FirstBit = SPI_FirstBit_MSB;
	SPIConf.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &SPIConf);
	//SPI_SSOutputCmd(SPI1, DISABLE);
	SPI_Cmd(SPI1, ENABLE);

	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, DISABLE);
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, DISABLE);
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_ERR, DISABLE);

	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, DISABLE);

	// Init DMA structure
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	// Init DMA Channel (MEM->SPI)
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);

	// Init DMA Channel (SPI->MEM)
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;

	DMA_Init(DMA1_Channel2, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);

//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_3_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	// spi1 ISR
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	spi_cs(0);
}

void spi_deinit(void)
{
	SPI_I2S_DeInit(SPI1);
	DMA_DeInit(DMA1_Channel2);
	DMA_DeInit(DMA1_Channel3);
}

static void spi_cs(uint8_t state)
{
	const struct gpio_init_table_t *cs;

	cs = &spi_cs_gpio[spi_csn];

	if (state) {
		GPIO_WriteBit(cs->gpio, cs->pin, Bit_RESET);
	} else {
		GPIO_WriteBit(cs->gpio, cs->pin, Bit_SET);
	}
}

uint8_t spi_is_done()
{
	return spi_done;
}

uint8_t spi_is_locked(void)
{
	return spi_locked;
}

uint8_t spi_lock(uint8_t cs)
{
	if (spi_locked == 1) {
		return 1;
	}

	spi_locked = 1;
	spi_csn = cs;
	spi_done = 0;

	return 0;
}

uint8_t spi_unlock(void)
{
	spi_locked = 0;
	return 0;
}

void spi_cs_on(void)
{
	spi_cs(1);
}

void spi_cs_off(void)
{
	spi_cs(0);
}

uint8_t spi_wait_miso(uint8_t state)
{
	volatile uint32_t time = 0xFFFFFF;
	struct gpio_init_table_t gpio_miso = spi_gpio[GPIO_MISO];


	while (--time && ((!!(gpio_miso.gpio->IDR & gpio_miso.pin)) != (!!state)));

	return (time > 0)?0:1;
}

uint8_t spi_send_slow(const uint8_t *outbuf, uint8_t *inbuf, uint32_t size)
{
	uint32_t i;
	volatile uint8_t dummy = 0;
	(void)dummy;

	for (i = 0; i < size; i ++) {
		while (!(SPI1->SR & SPI_I2S_FLAG_TXE));
		if (outbuf) {
			SPI_SendData8(SPI1, outbuf[i]);
		} else {
			SPI_SendData8(SPI1, 0xff);
		}
		while (!(SPI1->SR & 0x300));
		if (inbuf) {
			inbuf[i] = SPI_ReceiveData8(SPI1);
		} else {
			dummy = SPI_ReceiveData8(SPI1);
		}
	}

	return 0;
}

uint8_t spi_send(uint8_t *outbuf, uint8_t *inbuf, uint32_t size)
{
	spi_done = 0;

	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);

	if (inbuf == NULL) {
		DMA1_Channel2->CMAR = (uint32_t)&spi_dummy;
		DMA1_Channel2->CCR &= ~(DMA_CCR_MINC);
	} else {
		DMA1_Channel2->CMAR = (uint32_t)inbuf;
		DMA1_Channel2->CCR |= DMA_CCR_MINC;
	}
	DMA1_Channel2->CNDTR = size;
	DMA_Cmd(DMA1_Channel2, ENABLE);

	if (outbuf != NULL) {
		SPI1->DR = outbuf[0];

		if (size > 1) {
			DMA1_Channel3->CMAR = (uint32_t)&outbuf[1];
			DMA1_Channel3->CCR |= DMA_CCR_MINC; 
			DMA1_Channel3->CNDTR = size - 1;

			DMA_Cmd(DMA1_Channel3, ENABLE);
		} else {
			//SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
			//SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, ENABLE);
		}
	} else {
		SPI1->DR = spi_dummy;

		if (size > 1) {
			DMA1_Channel3->CMAR = (uint32_t)&spi_dummy;
			DMA1_Channel3->CNDTR = size - 1;
			DMA1_Channel3->CCR &= ~(DMA_CCR_MINC);

			DMA_Cmd(DMA1_Channel3, ENABLE);
		}
	}

	return 0;
}

void spi_wait_done()
{
	while (spi_done == 0);
}

void spi_send_blocking(uint8_t *outbuf, uint8_t *inbuf, uint32_t size)
{
	spi_send(outbuf, inbuf, size);
	spi_wait_done();
}

void SPI1_IRQHandler(void)
{
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, DISABLE);
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, DISABLE);
}

void DMA1_Channel2_3_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA1_IT_TC2)) {
		SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);

		DMA_ClearITPendingBit(DMA1_IT_GL2);
		DMA_Cmd(DMA1_Channel2, DISABLE);

		spi_done = 1;
	}

	if (DMA_GetITStatus(DMA1_IT_TC3)) {
		SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);

		DMA_ClearITPendingBit(DMA1_IT_GL3);
		DMA_Cmd(DMA1_Channel3, DISABLE);
	}
}


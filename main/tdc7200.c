#include "platform.h"

#include "console.h"
#include "gpio.h"
#include "spi.h"
#include "systime.h"

#include "cfg.h"

#include "tdc7200.h"

const struct gpio_init_table_t tdc_gpio[] = {
	{ // en A
		.gpio = GPIOB,
		.pin = GPIO_Pin_4,
		.mode = GPIO_Mode_OUT,
		.otype = GPIO_OType_PP,
		.speed = GPIO_Speed_Level_3,
		.state = GPIO_RESET,
	},
	{ // en B
		.gpio = GPIOA,
		.pin = GPIO_Pin_1,
		.mode = GPIO_Mode_OUT,
		.otype = GPIO_OType_PP,
		.speed = GPIO_Speed_Level_3,
		.state = GPIO_RESET,
	},
};

struct tdc7200_data_t tdc7200_data[2];

uint8_t tdc7200_config1[2];

static void tdc7200_reg_write(int cs, uint8_t reg, uint8_t val);
static uint8_t tdc7200_reg_read(int cs, uint8_t reg);
static uint32_t tdc7200_reg_read24(int cs, uint8_t reg);

void tdc7200_init(void)
{
	spi_init();
	GPIO_InitBatch(tdc_gpio, ARRAY_SIZE(tdc_gpio));
}

void tdc7200_stop(int cs)
{
	// disable TDC
	GPIO_Set(&tdc_gpio[cs], GPIO_RESET);
	systime_delay(5);
}

void tdc7200_setup(int cs, struct cfg_t *cfg)
{
	// reset TDC
	GPIO_Set(&tdc_gpio[cs], GPIO_RESET);
	systime_delay(5);
	GPIO_Set(&tdc_gpio[cs], GPIO_SET);
	systime_delay(5);

	uint8_t config2 = 0x00;
	switch (cfg->cal_periods) {
		case 2: config2 |= 0x00; break;
		case 10: config2 |= 0x40; break;
		case 20: config2 |= 0x80; break;
		case 40: config2 |= 0xC0; break;
	}
	tdc7200_reg_write(cs, TDC7200_CONFIG2, config2);

	tdc7200_reg_write(cs, TDC7200_CLOCK_CNTR_OVF_H, cfg->timeout);
	tdc7200_reg_write(cs, TDC7200_CLOCK_CNTR_OVF_L, 0x00);

	tdc7200_reg_write(cs, TDC7200_INT_MASK, 1);

	uint8_t config1 = 0;

	if (cfg->tdc[cs].start_edge) {
		config1 |= 0x08;
	}

	config1 |= 0x80 | 0x02 | 0x01;

	tdc7200_config1[cs] = config1;
	
	tdc7200_data[cs].calcount = 0;
}

void tdc7200_ready(int cs)
{
	tdc7200_reg_write(cs, TDC7200_CONFIG1, tdc7200_config1[cs]);
}

int64_t tdc7200_read(int cs, struct cfg_t *cfg)
{
//	int32_t time1, time2, clock1, cal1, cal2;
	int32_t time2;
	int32_t ring_ticks;
	int64_t normlsb, calcount, ring_ps, tof;

	//console_printf(CON_ERR, "tdc%d: status = %08x\n", cs, tdc7200_reg_read(cs, TDC7200_INT_STATUS));

	tdc7200_data[cs].time1 = tdc7200_reg_read24(cs, TDC7200_TIME1);
	tdc7200_data[cs].time2 = tdc7200_reg_read24(cs, TDC7200_TIME2);
	tdc7200_data[cs].clock1 = tdc7200_reg_read24(cs, TDC7200_CLOCK_COUNT1);
	tdc7200_data[cs].cal1 = tdc7200_reg_read24(cs, TDC7200_CALIBRATION1);
	tdc7200_data[cs].cal2 = tdc7200_reg_read24(cs, TDC7200_CALIBRATION2);

	tof = (int64_t)tdc7200_data[cs].clock1 * cfg->clock_period;
	tof -= (int64_t)cfg->tdc[cs].fudge0;

	calcount = ((int64_t)(tdc7200_data[cs].cal2 - tdc7200_data[cs].cal1) * (uint64_t)(1000000 - cfg->tdc[cs].time_dilatation)) / (int64_t)(cfg->cal_periods - 1);

	if (cfg->tdc[cs].fixed_time2) {
		time2 = cfg->tdc[cs].fixed_time2;
	} else {
		time2 = tdc7200_data[cs].time2;
	}

#if 0
	// calibration filtering
	if (tdc7200_data[cs].calcount == 0) {
		// first run
		tdc7200_data[cs].calcount = calcount;
	}
	
	//calcount = tdc7200_data[cs].calcount + ((calcount - tdc7200_data[cs].calcount) * 650) / 1000;
	calcount = (9999 * tdc7200_data[cs].calcount + 1 * calcount) / 10000;
#endif
	tdc7200_data[cs].calcount = calcount;

	normlsb = ((int64_t)cfg->clock_period * (int64_t)1000000000000 ) / calcount; 
	ring_ticks = tdc7200_data[cs].time1 - time2;
	ring_ps = (normlsb * (int64_t)ring_ticks) / (int64_t)1000000;
	tof += ring_ps;

	return tof;
}

static void tdc7200_reg_write2(int cs, uint8_t reg, uint8_t val)
{
	reg &= 0x1f;
	reg |= 0x40;

	spi_lock(cs);
	spi_cs_on();

	spi_send_slow(&reg, NULL, 1);
	spi_send_slow(&val, NULL, 1);

	spi_cs_off();
	spi_unlock();
}

static void tdc7200_reg_write(int cs, uint8_t reg, uint8_t val)
{
	tdc7200_reg_write2(cs, reg, val);
	uint8_t v = tdc7200_reg_read(cs, reg);
	if (v != val) {
		console_printf(CON_ERR, "tdc%d: error writing reg %02x: %02x != %02x\n", reg, val, v);
	}
}

static uint8_t tdc7200_reg_read(int cs, uint8_t reg)
{
	uint8_t d;

	reg &= 0x1f;

	spi_lock(cs);
	spi_cs_on();

	spi_send_slow(&reg, NULL, 1);
	spi_send_slow(NULL, &d, 1);

	spi_cs_off();
	spi_unlock();

	return d;
}

static uint32_t tdc7200_reg_read24(int cs, uint8_t reg)
{
	uint8_t d[3];

	reg &= 0x1f;

	spi_lock(cs);
	spi_cs_on();

	spi_send_slow(&reg, NULL, 1);
	spi_send_slow(NULL, d, 3);

	spi_cs_off();
	spi_unlock();


	return (d[0] << 16) | (d[1] << 8) | d[2];
}

#include "platform.h"

#include "console.h"
#include "gpio.h"
#include "led.h"
#include "exti.h"
#include "systime.h"

#include "cfg.h"
#include "tdc7200.h"

#include "ticc.h"

#define PS_PER_SEC                ((int64_t)1000000000000)   // ps/s

const struct gpio_init_table_t ticc_gpio[] = {
	{ // coarse timer
		.gpio = GPIOB,
		.pin = GPIO_Pin_7,
		.mode = GPIO_Mode_IN,
		.pupd = GPIO_PuPd_NOPULL,
	},
	{ // stop A
		.gpio = GPIOB,
		.pin = GPIO_Pin_5,
		.mode = GPIO_Mode_IN,
		.pupd = GPIO_PuPd_NOPULL,
	},
	{ // stop B
		.gpio = GPIOB,
		.pin = GPIO_Pin_6,
		.mode = GPIO_Mode_IN,
		.pupd = GPIO_PuPd_NOPULL,
	},
	{ // INTB A
		.gpio = GPIOA,
		.pin = GPIO_Pin_0,
		.mode = GPIO_Mode_IN,
		.pupd = GPIO_PuPd_NOPULL,
	},
	{ // INTB B
		.gpio = GPIOA,
		.pin = GPIO_Pin_2,
		.mode = GPIO_Mode_IN,
		.pupd = GPIO_PuPd_NOPULL,
	},
};

volatile uint64_t ticc_coarse_count;
struct ticc_chan_t ticc_chan[2];
static uint64_t ticc_last_coarse = 0;
static uint32_t ticc_last_check = 0;
static int ticc_coarse_lost = 0;

static void ticc_coarse_inthandler(void);
static void ticc_stopA_inthandler(void);
static void ticc_stopB_inthandler(void);
static struct tdc7200_cfg_t tdc_cfg[2];

static void ticc_restart_measurement(void);

void ticc_init(void)
{
	GPIO_InitBatch(ticc_gpio, ARRAY_SIZE(ticc_gpio));
	cfg_init();
	tdc7200_init();

	ticc_coarse_count = 0;

	exti_set_handler(EXTI_7, ticc_coarse_inthandler);
	exti_set_handler(EXTI_5, ticc_stopA_inthandler);
	exti_set_handler(EXTI_6, ticc_stopB_inthandler);
	exti_enable(EXTI_7, EXTI_Trigger_Falling, EXTI_PortSourceGPIOB);
	exti_enable(EXTI_5, EXTI_Trigger_Rising, EXTI_PortSourceGPIOB);
	exti_enable(EXTI_6, EXTI_Trigger_Rising, EXTI_PortSourceGPIOB);

	// wait for ticc_coarse_count to rise
	ticc_last_coarse = ticc_coarse_count;
	uint32_t count = 0;
	while(1) {
		systime_delay(10);
		if (ticc_last_coarse == ticc_coarse_count) {
			count++;
			if (count > 10) {
				console_printf(CON_ERR, "# reference clock probably missing\n");
				ticc_coarse_lost = 1;
				return;
			}
		} else {
			console_printf(CON_ERR, "# reference clock ok\n");
			ticc_coarse_lost = 0;
			return;
		}
	}

	ticc_restart_measurement();
}

static void ticc_stop()
{
	for (int i = 0; i < 2; i++) {
		tdc7200_stop(i);
	}
}

static void ticc_restart_measurement()
{
	ticc_stop();

	// reset counters
	memset(&ticc_chan, 0, sizeof(struct ticc_chan_t)*2);

	for (int i = 0; i < 2; i++) {
		tdc_cfg[i].start_edge = cfg_current.tdc[i].start_edge;
		tdc_cfg[i].time_dilatation = cfg_current.tdc[i].time_dilatation;
		tdc_cfg[i].fixed_time2 = cfg_current.tdc[i].fixed_time2;
		tdc_cfg[i].fudge0 = cfg_current.tdc[i].fudge0;
		tdc_cfg[i].clock_period = PS_PER_SEC / cfg_current.clock_hz;
		tdc_cfg[i].cal_periods = cfg_current.cal_periods;
		tdc_cfg[i].timeout = cfg_current.timeout;
	}

	// start measurement
	for (int i = 0; i < 2; i++) {
		tdc7200_setup(i, &tdc_cfg[i]);
		tdc7200_ready(i);
	}

	switch (cfg_current.mode) {
		case MODE_TIMESTAMP:
			console_printf(CON_ERR, "# timestamp (seconds)\n");
			break;
		case MODE_INTERVAL:
			console_printf(CON_ERR, "#time interval A->B (seconds)\n");
			break;
		case MODE_PERIOD:
			console_printf(CON_ERR, "# period (seconds)\n");
			break;
		case MODE_TIMELAB:
			console_printf(CON_ERR, "# timestamp chA, chB; interval chA->B (seconds)\n");
			break;
		case MODE_DEBUG:
			console_printf(CON_ERR, "# time1, time2, clock, cal1, cal2\n");
			break;
	}
}

void ticc_to_str_signed(char *str, int64_t x)
{
	uint64_t sec, frac;

	sec  = ABS(x / PS_PER_SEC);
	frac = ABS(x % PS_PER_SEC);

	if (x < 0) {
		str[0] = '-';
		str++;
	}

	tfp_sprintf(str, "%llu.%012llu", sec, frac);
}

void ticc_to_str_unsigned(char *str, uint64_t x)
{
	uint64_t sec, frac;

	sec  = ABS(x / PS_PER_SEC);
	frac = ABS(x % PS_PER_SEC);

	tfp_sprintf(str, "%llu.%012llu", sec, frac);
}

void ticc_periodic(void)
{
	uint64_t clock_period; // calculated
	uint64_t pictick_ps;

	clock_period = PS_PER_SEC / cfg_current.clock_hz;
	pictick_ps = clock_period * 1000; // 10Mhz -> 100kHz

	// check reference clock
	if (systime_get() - ticc_last_check > SYSTIME_SEC(1)) {
		ticc_last_check = systime_get();

		if (ticc_last_coarse == ticc_coarse_count) {
			if (ticc_coarse_lost == 0) {
				ticc_coarse_lost = 1;
				console_printf(CON_ERR, "# reference clock lost! stopping measurement\n");

				ticc_stop();
			}
		} else {
			ticc_last_coarse = ticc_coarse_count;
			if (ticc_coarse_lost == 1) {
				ticc_coarse_lost = 0;
				console_printf(CON_ERR, "# reference clock ready! restarting measurement\n");

				ticc_restart();
			}
		}
	}

	for (int i = 0; i < 2; i++) {
		if (GPIO_Get(&ticc_gpio[i+3]) == 1) {
			// no interrupt yet
			continue;
		}

		led_toggle(i);

		ticc_chan[i].last_tof = ticc_chan[i].tof;
		ticc_chan[i].last_ts = ticc_chan[i].ts;
		ticc_chan[i].tof = tdc7200_read(i, &tdc_cfg[i]);
		ticc_chan[i].ts = (ticc_chan[i].pic_stop * pictick_ps) - ticc_chan[i].tof;
		ticc_chan[i].period = ticc_chan[i].ts - ticc_chan[i].last_ts;
		ticc_chan[i].totalize ++;

		tdc7200_ready(i);

		if (ticc_chan[i].totalize > 2) {
			char str[32];
			switch (cfg_current.mode) {
				case MODE_TIMESTAMP:
					ticc_to_str_unsigned(str, ticc_chan[i].ts);
					console_printf(CON_ERR, "%s ch%c\n", str, (i==0) ? 'A' : 'B');
					break;
				case MODE_INTERVAL:
					if ((ticc_chan[0].ts == 0) || (ticc_chan[1].ts == 0)) {
						break;
					}
					/*ticc_to_str_unsigned(str, ticc_chan[0].ts);
					console_printf(CON_ERR, "%s TI(A) ", str);
					ticc_to_str_unsigned(str, ticc_chan[1].ts);
					console_printf(CON_ERR, "%s TI(B) ", str);*/
					ticc_to_str_signed(str, ticc_chan[1].ts - ticc_chan[0].ts);
					console_printf(CON_ERR, "%s TI(A->B)\n", str);
					ticc_chan[0].ts = 0;
					ticc_chan[1].ts = 0;
					break;
				case MODE_PERIOD:
					ticc_to_str_signed(str, ticc_chan[i].period);
					console_printf(CON_ERR, "%s ch%c\n", str, (i==0) ? 'A' : 'B');
					break;
				case MODE_TIMELAB:
					if ((ticc_chan[0].ts == 0) || (ticc_chan[1].ts == 0)) {
						break;
					}
					ticc_to_str_unsigned(str, ticc_chan[0].ts);
					console_printf(CON_ERR, "%s chA\n", str);
					ticc_to_str_unsigned(str, ticc_chan[1].ts);
					console_printf(CON_ERR, "%s chB\n", str);
					ticc_to_str_unsigned(str, (ticc_chan[1].ts - ticc_chan[0].ts) +  ( (ticc_chan[1].totalize * (int64_t)PS_PER_SEC) - 1) );
					console_printf(CON_ERR, "%s chC(B-A)\n", str);
					ticc_chan[0].ts = 0;
					ticc_chan[1].ts = 0;

					break;
				case MODE_DEBUG:
					console_printf(CON_ERR, "%06d %06d %06d %06d %06d %06llu %06llu ch%c\n",
							tdc7200_data[i].time1,
							tdc7200_data[i].time2,
							tdc7200_data[i].clock1,
							tdc7200_data[i].cal1,
							tdc7200_data[i].cal2,
							tdc7200_data[i].calcount,
							ticc_chan[i].tof,
							(i==0) ? 'A' : 'B');
					break;
				default:
					break;
			}
		}
		
		led_toggle(i);
	}
}

static void ticc_coarse_inthandler(void)
{
	ticc_coarse_count ++;
}

static void ticc_stopA_inthandler(void)
{
	ticc_chan[0].pic_stop = ticc_coarse_count;
}

static void ticc_stopB_inthandler(void)
{
	ticc_chan[1].pic_stop = ticc_coarse_count;
}

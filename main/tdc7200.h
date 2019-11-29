#ifndef TDC7200_h_
#define TDC7200_h_

#include "cfg.h"

#define TDC7200_CONFIG1              0x00
#define TDC7200_CONFIG2              0x01
#define TDC7200_INT_STATUS           0x02
#define TDC7200_INT_MASK             0x03
#define TDC7200_COARSE_CNTR_OVF_H    0x04
#define TDC7200_COARSE_CNTR_OVF_L    0x05
#define TDC7200_CLOCK_CNTR_OVF_H     0x06
#define TDC7200_CLOCK_CNTR_OVF_L     0x07
#define TDC7200_CLOCK_CNTR_STOP_H    0x08
#define TDC7200_CLOCK_CNTR_STOP_L    0x09

#define TDC7200_TIME1                0x10
#define TDC7200_CLOCK_COUNT1         0x11
#define TDC7200_TIME2                0x12
#define TDC7200_CLOCK_COUNT2         0x13
#define TDC7200_TIME3                0x14
#define TDC7200_CLOCK_COUNT3         0x15
#define TDC7200_TIME4                0x16
#define TDC7200_CLOCK_COUNT4         0x17
#define TDC7200_TIME5                0x18
#define TDC7200_CLOCK_COUNT5         0x19
#define TDC7200_TIME6                0x1A
#define TDC7200_CALIBRATION1         0x1B
#define TDC7200_CALIBRATION2         0x1C

struct tdc7200_data_t {
	int32_t time1;
	int32_t time2;
	int32_t clock1;
	int32_t cal1;
	int32_t cal2;

	int64_t calcount;
};

extern struct tdc7200_data_t tdc7200_data[2];

void tdc7200_init(void);
void tdc7200_stop(int cs);
void tdc7200_setup(int cs, struct cfg_t *cfg);
void tdc7200_ready(int cs);
int64_t tdc7200_read(int cs, struct cfg_t *cfg);

#endif

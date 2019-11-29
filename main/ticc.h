#ifndef TICC_h_
#define TICC_h_

enum {
	MODE_STOP,
	MODE_TIMESTAMP,
	MODE_INTERVAL,
	MODE_PERIOD,
	MODE_TIMELAB,
	MODE_DEBUG,
};

struct ticc_chan_t {
	uint64_t totalize;
	uint64_t pic_stop;
	uint64_t tof;
	uint64_t last_tof;
	uint64_t ts;
	uint64_t last_ts;
	uint64_t period;
};

void ticc_init(void);
void ticc_periodic(void);
#endif

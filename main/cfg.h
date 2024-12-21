#ifndef CFG_h_
#define CFG_h_



enum {
	TYPE_END,
	TYPE_LABEL,
	TYPE_BIT,
	TYPE_NUM,
	TYPE_SNUM,
	TYPE_IPADDR,
	TYPE_ARRAY,
	TYPE_STRING,
	TYPE_PASSWD,
	TYPE_CHAR,
};

union anyvalue {
	uint8_t bytes[4];
	char ch;
	uint8_t u8;
	uint16_t u16;
	uint32_t u32;
	uint64_t u64;
	int8_t s8;
	int16_t s16;
	int32_t s32;
	int64_t s64;
	void *ptr;
};

struct config_desc_t {
	uint32_t offset;
	uint8_t type;
	uint32_t shift;
	uint32_t param;
	char *str;
	char *comment;
};

struct cfg_tdc_t {
	uint32_t start_edge; // 0 - rising edge, 1 - falling edge
	int32_t time_dilatation;
	int32_t fixed_time2;
	int64_t fudge0;
};

struct cfg_t {
	uint32_t magic;
	uint32_t mode;
	char poll_char;
	uint64_t clock_hz;
	uint32_t cal_periods;
	uint32_t timeout;
	struct cfg_tdc_t tdc[2];
	uint16_t chksum;
};

extern struct cfg_t cfg_current;

void cfg_init(void);
#endif

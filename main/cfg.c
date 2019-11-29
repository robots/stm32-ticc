#include "platform.h"
#include "console.h"

#include "cfg.h"

#define CFG_ADDR 0x08007c00
#define CFG_STRUCT ((struct cfg_t *)CFG_ADDR)

static struct console_command_t cfg_cmd;

struct cfg_t cfg_current;
struct cfg_t cfg_default = {
	.mode = 0,
	.poll_char = 0,
	.clock_hz = 10000000,
	.pictick_ps = 100000000,
	.cal_periods = 20,
	.timeout = 0x05,
	.tdc = {
		{
			.start_edge = 0,
			.time_dilatation = 2500,
			.fixed_time2 = 0,
			.fudge0 = 0,
		},
		{
			.start_edge = 0,
			.time_dilatation = 2500,
			.fixed_time2 = 0,
			.fudge0 = 0,
		}
	},
};

static uint16_t cfg_chksum(struct cfg_t *cfg)
{
	uint8_t *data = (uint8_t *)cfg;
	uint16_t sum = 0;

	for (uint8_t i = 0; i < sizeof(struct cfg_t)-2; i++) {
		sum += data[i];
	}

	return sum;
}

void cfg_init(void)
{
	cfg_current = cfg_default;

	if (CFG_STRUCT->magic == 0xDEADBEEF) {
		if (CFG_STRUCT->chksum == cfg_chksum(CFG_STRUCT)) {
			memcpy(&cfg_current, CFG_STRUCT, sizeof(struct cfg_t));
		}
	}

	cfg_current.clock_period = (uint64_t)1000000000000 / cfg_current.clock_hz;
	cfg_current.pictick_ps = cfg_current.clock_period * 1000; // 10Mhz -> 100kHz
	if ((cfg_current.cal_periods != 2) && (cfg_current.cal_periods != 10) && (cfg_current.cal_periods != 20) && (cfg_current.cal_periods != 40)) {
		cfg_current.cal_periods = cfg_default.cal_periods;
	}

	console_add_command(&cfg_cmd);
}

static void cfg_save(struct cfg_t *cfg)
{
	uint32_t *data = (uint32_t *)cfg;
	cfg->magic = 0xDEADBEEF;
	cfg->chksum = cfg_chksum(cfg);

	FLASH_Unlock();
	FLASH_ErasePage(CFG_ADDR);
	for (uint32_t i = 0; i < sizeof(struct cfg_t); i += 4) {
		FLASH_ProgramWord(CFG_ADDR + i, data[i]);
	}

	FLASH_Lock();
}

void cfg_print(struct console_session_t *cs, const struct config_desc_t *cd, struct cfg_t *cfg)
{
	char out[200];
	uint32_t len = 0;
	union anyvalue val;
	void *data = cfg;

	while (cd->type != TYPE_END) {
		out[0] = 0;
		len = 0;

		memcpy(val.bytes, data+cd->offset, 4);

		if (cd->type == TYPE_LABEL) {
			len = tfp_sprintf(out, "*** %s", cd->str);
		} else {
			len = tfp_sprintf(out, "%s = ", cd->str);
			//n = tfp_sprintf(out, "%s = %08x %08x %d %d", cd->str, cd->offset, (uint32_t)data+cd->offset, cd->type, 0);

			if (cd->type == TYPE_NUM) {
				uint32_t num = 0;
				if (cd->param != 64) {
					if (cd->param == 8) {
						num = val.u8;
					} else if (cd->param == 16) {
						num = val.u16;
					} else if (cd->param == 32) {
						num = val.u32;
					}
					len += tfp_sprintf(out+len, "%d", num);
				} else {
					uint64_t n = *(uint64_t *)((uint32_t)data + cd->offset);
					len += tfp_sprintf(out+len, "%llu", n);
				}
			} else if (cd->type == TYPE_SNUM) {
				if (cd->param != 64) {
					int32_t num = 0;
					if (cd->param == 8) {
						num = val.s8;
					} else if (cd->param == 16) {
						num = val.s16;
					} else if (cd->param == 32) {
						num = val.s32;
					}
					len += tfp_sprintf(out+len, "%d", num);
				} else {
					uint64_t n = *(uint64_t *)((uint32_t)data + cd->offset);
					len += tfp_sprintf(out+len, "%lld", n);
				}
			} else if (cd->type == TYPE_BIT) {
				uint8_t state = 0;
				if (cd->param == 8) {
					state = val.u8 & (1 << cd->shift);
				} else if (cd->param == 16) {
					state = val.u16 & (1 << cd->shift);
				} else if (cd->param == 32) {
					state = val.u32 & (1 << cd->shift);
				}
				len += tfp_sprintf(out+len, "%d", !!state);
			} else if (cd->type == TYPE_ARRAY) {
				uint8_t *ary = (uint8_t *)((uint32_t)data + cd->offset);
				for (uint32_t j = 0; j < cd->param; j++) {
					len += tfp_sprintf(out+len, "%02x", ary[j]);
					if (j < cd->param-1)
						len += tfp_sprintf(out+len, ":");
				}
			} else if (cd->type == TYPE_STRING) {
				char *str = (char *)((uint32_t)data + cd->offset);
				for (uint32_t j = 0; j < cd->param; j++) {
					if (str[j] == '\0') {
						break;
					}
					len += tfp_sprintf(out+len, "%c", str[j]);
				}
			} else if (cd->type == TYPE_PASSWD) {
				for (uint32_t j = 0; j < cd->param; j++) {
					len += tfp_sprintf(out+len, "*");
				}
			} else if (cd->type == TYPE_CHAR) {
				len += tfp_sprintf(out+len, "%c", val.ch);
			}
		}

		if (cd->comment) {
			len += tfp_sprintf(out+len, "\t%s", cd->comment);
		}

		len += tfp_sprintf(out+len, "\n");
		console_session_output(cs, out, len);
		cd++;
	}
}

int cfg_alter(const struct config_desc_t *config_desc, void *data, char *param, char *value)
{
	const struct config_desc_t *cd = NULL;
	uint32_t i = 0;
	union anyvalue val;

	while (1) {
		if (config_desc[i].str == NULL) {
			return 2;
		}

		if (strcmp(param, config_desc[i].str) == 0) {
			cd = &config_desc[i];
			break;
		}
		i++;
	}

	if (cd == NULL) {
		return 2;
	}

	if ((cd->type == TYPE_NUM) || (cd->type == TYPE_SNUM) || (cd->type == TYPE_BIT)) {
		memcpy(val.bytes, data+cd->offset, 4);
		if (cd->type == TYPE_NUM) {
			char *end;
			if (config_desc[i].param == 8) {
				val.u8 = strtol(value, &end, 0);
			} else if (config_desc[i].param == 16) {
				val.u16 = strtol(value, &end, 0);
			} else if (config_desc[i].param == 32) {
				val.u32 = strtol(value, &end, 0);
			} else if (config_desc[i].param == 64) {
				val.u64 = strtoll(value, &end, 0);
			}
			if (*end != 0) {
				//console_session_printf(cs, "Number parse error\n");
				return 1;
			}
		} else if (cd->type == TYPE_SNUM) {
			char *end;
			if (config_desc[i].param == 8) {
				val.s8 = strtol(value, &end, 0);
			} else if (config_desc[i].param == 16) {
				val.s16 = strtol(value, &end, 0);
			} else if (config_desc[i].param == 32) {
				val.s32 = strtol(value, &end, 0);
			} else if (config_desc[i].param == 64) {
				val.s64 = strtoll(value, &end, 0);
			}
			if (*end != 0) {
				//console_session_printf(cs, "Number parse error\n");
				return 1;
			}
		} else if (cd->type == TYPE_BIT) {
			uint8_t state = atoi(value)?1:0;
			uint32_t mask = (1 << config_desc[i].shift);
			if (config_desc[i].param == 8) {
				if (state) val.u8 |= mask; else	val.u8 &= ~mask;
			} else if (config_desc[i].param == 16) {
				if (state) val.u16 |= mask; else val.u16 &= ~mask;
			} else if (config_desc[i].param == 32) {
				if (state) val.u32 |= mask; else val.u32 &= ~mask;
			}
		}
		memcpy(data+cd->offset, val.bytes, 4);
		return 0;
	} else if (cd->type == TYPE_ARRAY) {
		//console_session_printf(cs, "Array alteration not implemented\n");
		return 3;
	} else if ((cd->type == TYPE_STRING) || (cd->type == TYPE_PASSWD)) {
		char *str = (char *)((uint32_t)data + config_desc[i].offset);
		memset(str, 0, cd->param);
		memcpy(str, value, MIN(strlen(value), cd->param));
		return 0;
	} else if (cd->type == TYPE_CHAR) {
		char *ch = (char *)((uint32_t)data + config_desc[i].offset);
		*ch = value[0];
		return 0;
	}

	return 1;
}

const struct config_desc_t config_desc[] = {
	{                                  0,              TYPE_LABEL,    0,  0, "TICC", NULL},
	{offsetof(struct cfg_t, mode),                   TYPE_NUM,   0, 16, "mode", "0 - stop, 1 - timestamp, 2 - inteval, 3 - period, 4 - timelab, 5 - debug" },
//	{offsetof(struct cfg_t, poll_char),              TYPE_CHAR,  0,  8, "poll_char", ""},
	{offsetof(struct cfg_t, clock_hz),               TYPE_NUM,   0, 64, "clock_hz", "reference clock frequency in Hz"},
	{offsetof(struct cfg_t, cal_periods),            TYPE_NUM,   0, 32, "cal_periods", "2, 10, 20, 40"},
	{offsetof(struct cfg_t, timeout),                TYPE_NUM,   0, 32, "timeout", ""},
	{                                  0,              TYPE_LABEL,    0,  0, "Channel A", NULL},
	{offsetof(struct cfg_t, tdc[0].start_edge),      TYPE_NUM,   0, 32, "start_edge_A", "0 - rising, 1 - falling"},
	{offsetof(struct cfg_t, tdc[0].time_dilatation), TYPE_NUM,   0, 32, "time_dilatation_A", NULL},
	{offsetof(struct cfg_t, tdc[0].fixed_time2),     TYPE_NUM,   0, 32, "fixed_time2_A", NULL},
	{offsetof(struct cfg_t, tdc[0].fudge0),          TYPE_NUM,   0, 64, "fudge0_A", NULL},
	{                                  0,              TYPE_LABEL,    0,  0, "Channel B", NULL},
	{offsetof(struct cfg_t, tdc[0].start_edge),      TYPE_NUM,   0, 32, "start_edge_B", "0 - rising, 1 - falling"},
	{offsetof(struct cfg_t, tdc[0].time_dilatation), TYPE_NUM,   0, 32, "time_dilatation_B", NULL},
	{offsetof(struct cfg_t, tdc[0].fixed_time2),     TYPE_NUM,   0, 32, "fixed_time2_B", NULL},
	{offsetof(struct cfg_t, tdc[0].fudge0),          TYPE_NUM,   0, 64, "fudge0_B", NULL},
	{                                  0,            TYPE_END,             0,  0, NULL, NULL},
};

static uint8_t cfg_cmd_handler(struct console_session_t *cs, char **args)
{
	if (args[0] == NULL) {
		console_session_printf(cs, "Valid: show, set {item} {val}, revert, default, apply, status\n");
		return 1;
	}

	if (strcmp(args[0], "show") == 0) {
		console_session_printf(cs, "Current configuration\n");
		cfg_print(cs, config_desc, &cfg_current);

	} else if (strcmp(args[0], "set") == 0) {
		if ((args[1] == NULL) || (args[2] == NULL)) {
			return 1;
		}

		int alt = cfg_alter(config_desc, &cfg_current, args[1], args[2]);

		if (alt == 0) {
			console_session_printf(cs, "Parameter '%s' altered\n", args[1]);
		} else if (alt == 1) {
			console_session_printf(cs, "Error setting parameter '%s'\n", args[1]);
		} else if (alt == 2) {
			console_session_printf(cs, "Parameter '%s' does not exists\n", args[1]);
		} else {
			console_session_printf(cs, "Not implemented\n");
		}

	} else if (strcmp(args[0], "revert") == 0) {
		//cff_current = cfg_eeprom;
	} else if (strcmp(args[0], "default") == 0) {
		cfg_current = cfg_default;
	} else if (strcmp(args[0], "save") == 0) {
		cfg_save(&cfg_current);
	}

	return 0;
}

static struct console_command_t cfg_cmd = {
	"cfg",
	cfg_cmd_handler,
	"Configuration",
	"Configuration:\n revert - restore from eeprom\n default - restore from built-in defaults \n apply - apply current configuration\n save - save current configuration into NVMEM\n status - print current NVMEM status\n show - print current configuration \n set option value - sets option to value",
	NULL
};

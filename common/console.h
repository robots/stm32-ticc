#ifndef CONSOLE_h_
#define CONSOLE_h_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#if CONSOLE_HAVE_THREAD
#include "pt.h"
#endif
#include "printf.h"
#include "config.h"

#define CONSOLE_FLAG_ECHO 0x0001

enum {
	CON_ERR,
	CON_WARN,
	CON_INFO,
	CON_DEBUG,
	CON_LAST,
};

enum {
	CON_AUTH_USER,
	CON_AUTH_PASS,
	CON_AUTH_OK,
};

struct console_session_t;

typedef void (*session_output_t)(struct console_session_t *, const char *, uint32_t);
typedef void (*session_close_t)(struct console_session_t *);
typedef uint8_t (*cmd_fnc_t)(struct console_session_t *, char **);
typedef char (*console_thread_t)(struct console_session_t *);

struct console_command_t {
	char *cmd;
	cmd_fnc_t fnc;
	char *help_short;
	char *help_long;
	struct console_command_t *next;
};


struct console_session_t {
	uint8_t used:1;

	uint8_t is_escape:1;

	uint16_t flags;
	uint8_t auth_state;

	char cmd_buf[CONSOLE_CMD_BUFFER];
	char last_escape;
	uint8_t cmd_len;
	uint8_t verbosity;

	session_output_t output;
	session_close_t close;

#if CONSOLE_HAVE_THREAD
	console_thread_t ct;
	struct pt pt;
#endif

	void *priv;
};

struct console_stats_t {
	uint32_t offset;
	uint32_t shift;
	uint32_t mask;
	char *str;
};


void console_init(void);
void console_periodic(void);
void console_add_command(struct console_command_t *cmd);
void console_lock(void);
void console_unlock(void);
void cprintf(char *fmt, ...);
void console_print_hex(uint8_t level, const char *buf, uint32_t len);
void console_printf(uint8_t level, const char *fmt, ...);
void console_write(uint8_t level, const char *buf, uint32_t len);
void console_print_prompt(struct console_session_t *cs);
void console_session_output(struct console_session_t * cs, const char *buf, uint32_t len);
void console_session_printf(struct console_session_t *cs, const char *fmt, ...);
uint8_t console_session_init(struct console_session_t **cs, session_output_t output, session_close_t close);
void console_session_close(struct console_session_t *cs);

void console_cmd_parse(struct console_session_t *cs, char *buf, uint32_t len);



#endif

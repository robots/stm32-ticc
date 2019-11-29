#include "platform.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#include "config.h"

// for dfu
#include "flash.h"
#include "usbd_cdc_core.h"

#include "printf.h"
//#include "console_uart.h"
#if CONSOLE_HAVE_TELNET
#include "console_telnet.h"
#endif
#if HAVE_SSL
#include "console_stelnet.h"
#endif
#if CONSOLE_HAVE_DMESG
#include "console_dmesg.h"
#endif
#if CONSOLE_HAVE_THREAD
#include "console_thread.h"
#endif
#include "console.h"

#undef DEBUG

static uint8_t cmd_help(struct console_session_t *cs, char **args);
static uint8_t cmd_verbose(struct console_session_t *cs, char **args);
static uint8_t cmd_reboot(struct console_session_t *cs, char **args);
static uint8_t cmd_dfu(struct console_session_t *cs, char **args);

static void console_auth_process(struct console_session_t *cs);
static void console_cmd_process(struct console_session_t *cs);

struct console_session_t console_sessions[CONSOLE_SESSION_NUM];

struct console_command_t console_command[] = {
	{ "help",    cmd_help,  "Prints help", "Help:\n command - prints help about command", NULL },
	{ "verbose", cmd_verbose, "Verbosity", "Verbosity\n x - Set verbosity level for this terminal", NULL },
	{ "reboot",  cmd_reboot, "Reboot immediately", "Reboot:\n boot - reboots into bootloader", NULL },
	{ "dfu",     cmd_dfu,    "Enter mcu's DFU mode", "", NULL },
};

const char console_crlf[] = "\r\n";
const char console_bs[] = "\x08 \x08";

const char *console_prompts[] = {
	"User:",
	"Pass:",
	">",
};

void console_init(void)
{
	uint32_t i;

	struct console_command_t *cmd = &console_command[0];
	for (i = 1; i < ARRAY_SIZE(console_command); i++) {
		cmd->next = &console_command[i];
		if (cmd->cmd == NULL)
			continue;
		cmd = cmd->next;
	}

	for (i = 0; i < CONSOLE_SESSION_NUM; i++) {
		memset(&console_sessions[i], 0, sizeof(struct console_session_t));
	}

#if CONSOLE_HAVE_DMESG
	console_dmesg_init();
#endif
	//console_uart_init();
#if CONSOLE_HAVE_THREAD
	console_thread_init();
#endif
}

void console_periodic(void)
{
	uint32_t i;

	for (i = 0; i < CONSOLE_SESSION_NUM; i++) {
		if (console_sessions[i].used) {
			console_thread_handle(&console_sessions[i]);
		}
	}
}

void console_add_command(struct console_command_t *new_cmd)
{
	struct console_command_t *cmd = &console_command[0];

	if (new_cmd->next) {
		return;
	}

	while (cmd->next != NULL) {
		cmd = cmd->next;
	}

	cmd->next = new_cmd;
}

void console_lock(void)
{
	//console_uart_disable_int();
}

void console_unlock(void)
{
	//console_uart_enable_int();
}

void cprintf(char *fmt, ...)
{
	char out[CONSOLE_CMD_OUTBUF];
	uint32_t len;

	va_list va;
	va_start(va, fmt);
	len = tfp_vsprintf(out, fmt, va);
	va_end(va);

	console_write(CON_ERR, out, len);
}

void console_print_hex(uint8_t level, const char *buf, uint32_t buf_len)
{
	char out[CONSOLE_CMD_OUTBUF];
	uint32_t len = 0;
	uint32_t i;

	for (i = 0; i < buf_len; i++) {
		len += tfp_sprintf(out+len, "%02x ", buf[i]);
	}

	console_printf(level, "%s\n", out);
}

void console_printf(uint8_t level, const char *fmt, ...)
{
	char out[CONSOLE_CMD_OUTBUF];
	uint32_t len;

	va_list va;
	va_start(va, fmt);
	len = tfp_vsprintf(out, fmt, va);
	va_end(va);

	console_write(level, out, len);
}

static void console_output(uint8_t level, const char *buf, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < CONSOLE_SESSION_NUM; i++) {
		if (console_sessions[i].used) {
			if (console_sessions[i].auth_state == CON_AUTH_OK) {
				if (console_sessions[i].verbosity >= level) {
					console_sessions[i].output(&console_sessions[i], buf, len);
				}
			}
		}
	}
}

static void console_output_prompt(uint8_t level)
{
	uint32_t i;

	for (i = 0; i < CONSOLE_SESSION_NUM; i++) {
		if (console_sessions[i].used) {
			if (console_sessions[i].auth_state == CON_AUTH_OK) {
				if (console_sessions[i].verbosity >= level) {
					if (console_sessions[i].cmd_len) {
						console_print_prompt(&console_sessions[i]);

						console_sessions[i].output(&console_sessions[i], console_sessions[i].cmd_buf, console_sessions[i].cmd_len);
					}
				}
			}
		}
	}
}

static void console_output_cr(uint8_t level)
{
	uint32_t i;

	for (i = 0; i < CONSOLE_SESSION_NUM; i++) {
		if (console_sessions[i].used) {
			if (console_sessions[i].auth_state == CON_AUTH_OK) {
				if (console_sessions[i].verbosity >= level) {
					if (console_sessions[i].cmd_len > 0) {
						console_sessions[i].output(&console_sessions[i], "\r", 1);
					}
				}
			}
		}
	}
}
void console_write(uint8_t level, const char *buf, uint32_t len)
{
	uint32_t l = len;
	const char *start = buf;
	const char *end;

	console_output_cr(level);

	// rewrite all \n to \r\n
	while (l && ((end = strchr(start, '\n')) != NULL)) {
		uint32_t w = end-start;

		if (w > l) {
			w = l;
		}

		console_output(level, start, w);
		console_output(level, console_crlf, sizeof(console_crlf));

#if CONSOLE_HAVE_DMESG
		console_dmesg_putbuf(start, w);
		console_dmesg_putbuf(console_crlf, sizeof(console_crlf));
#endif

		l -= w+1;
		start = end+1;
	}

	if (l) {
		console_output(level, start, l);
#if CONSOLE_HAVE_DMESG
		console_dmesg_putbuf(start, l);
#endif
	}

	// finish writing to console by showing prompt
	console_output_prompt(level);
}

void console_session_output(struct console_session_t * cs, const char *buf, uint32_t len)
{
	uint32_t l = len;
	const char *start = buf;
	const char *end;

	// rewrite all \n to \r\n
	while (l && ((end = strchr(start, '\n')) != NULL)) {
		uint32_t w = end-start;

		if (w > l) {
			w = l;
		}

		cs->output(cs, start, w);
		cs->output(cs, console_crlf, sizeof(console_crlf));


		l -= w+1;
		start = end+1;
	}

	if (l) {
		cs->output(cs, start, l);
	}
}

void console_session_printf(struct console_session_t *cs, const char *fmt, ...)
{
	char out[CONSOLE_CMD_OUTBUF];
	uint32_t len;

	va_list va;
	va_start(va, fmt);
	len = tfp_vsprintf(out, fmt, va);
	va_end(va);

	console_session_output(cs, out, len);
}

uint8_t console_session_init(struct console_session_t **cs, session_output_t output, session_close_t close)
{
	uint32_t i;

	for (i = 0; i < CONSOLE_SESSION_NUM; i++) {
		if (console_sessions[i].used == 0) {
			break;
		}
	}

	if (i >= CONSOLE_SESSION_NUM) {
		return 1;
	}

	memset(&console_sessions[i], 0, sizeof(struct console_session_t));
	console_sessions[i].used = 1;
	console_sessions[i].output = output;
	console_sessions[i].close = close;
	console_sessions[i].verbosity = CON_ERR;

	*cs = &console_sessions[i];

	return 0;
}

void console_session_close(struct console_session_t *cs)
{
	cs->used = 0;
	cs->auth_state = 0;
	cs->flags = 0;
	cs->output = NULL;
}

void console_print_prompt(struct console_session_t *cs)
{
	const char *p;

	p = console_prompts[cs->auth_state];
	cs->output(cs, p, strlen(p));
}

void console_cmd_parse(struct console_session_t *cs, char *buf, uint32_t len)
{
	uint32_t i;
	char *beep = "\x07";

	for (i = 0; i < len; i++) {
#if DEBUG
		cprintf("in: %x is_escape: %d last excape: %x\n", buf[i], cs->is_escape, cs->last_escape);
#endif
		if (cs->is_escape) {
			if (cs->last_escape == '\x00') {
				if (buf[i] == '[') {
					cs->last_escape = buf[i];
				} else {
					cs->is_escape = 0;
				}
			} else if (cs->last_escape == '3') {
				if (buf[i] == '~') {
					// delete key
					cs->is_escape = 0;
				} else {
					cs->is_escape = 0;
				}
			} else if (cs->last_escape == '[') {
				if (buf[i] == 'A') {
					// key up
					cs->is_escape = 0;
				} else if (buf[i] == 'B') {
					// key down
					cs->is_escape = 0;
				} else if (buf[i] == 'C') {
					// key right
					cs->is_escape = 0;
				} else if (buf[i] == 'D') {
					// key left
					cs->is_escape = 0;
				} else if (buf[i] == '3') {
					cs->last_escape = buf[i];
				} else {
					cs->is_escape = 0;
				}
			} else {
				cs->is_escape = 0;
			}
		} else if (buf[i] == 0x1b) {
			// escape
			cs->is_escape = 1;
			cs->last_escape = '\x00';
		} else if (buf[i] == '\n') {
			continue;
		} else if (buf[i] == '\r') {
			cs->cmd_buf[cs->cmd_len] = 0;
			if (cs->flags & CONSOLE_FLAG_ECHO) {
				cs->output(cs, console_crlf, sizeof(console_crlf));
			}
			if (cs->cmd_len > 0) {
				if (cs->auth_state == CON_AUTH_OK) {
					console_cmd_process(cs);
				} else {
					console_auth_process(cs);
				}
			}
			console_print_prompt(cs);
			cs->cmd_len = 0;
		} else if (buf[i] == 0x08) {
			// backspace
			cs->cmd_buf[cs->cmd_len] = 0;
			if (cs->cmd_len > 0) {
				cs->cmd_len -= 1;

				if (cs->flags & CONSOLE_FLAG_ECHO) {
					cs->output(cs, console_bs, sizeof(console_bs));
				}
			}
			continue;
		} else if (isprint((int)buf[i])) {
			if (cs->cmd_len < CONSOLE_CMD_BUFFER - 1) {
				cs->cmd_buf[cs->cmd_len] = buf[i];
				cs->cmd_len += 1;

				if (cs->flags & CONSOLE_FLAG_ECHO) {
					cs->output(cs, &buf[i], 1);
				}
			} else {
				cs->output(cs, beep, strlen(beep));
			}
		} else {
//			cs->cmd_len = 0;
		}
	}
}

static struct console_command_t *console_cmd_find(struct console_session_t *cs, char *buf, uint8_t len)
{
	struct console_command_t *cmd = &console_command[0];
	struct console_command_t *out = NULL;

	while (cmd != NULL) {
		if (strncmp(buf, cmd->cmd, len) == 0) {
			if (out == NULL) {
				out = cmd;
			} else {
				char *msg = "Command too ambigous\n";
				console_session_output(cs, msg, strlen(msg));
				return NULL; // second match, too ambigous
			}
		}
		cmd = cmd->next;
	}

	if (out == NULL) {
		char *msg = "Command not found\n";
		console_session_output(cs, msg, strlen(msg));
	}

	return out;
}

static void console_auth_process(struct console_session_t *cs)
{
/*	if (cs->auth_state == CON_AUTH_USER) {
		if (strlen(ee_config.password) == 0) {
			cs->auth_state = CON_AUTH_OK;
			return;
		}
		if (strncmp(ee_config.username, (char *)cs->cmd_buf, EE_USERNAME_LEN) == 0) {
			if (strlen(ee_config.password) == 0) {
				cs->auth_state = CON_AUTH_OK;
			} else {
				cs->auth_state = CON_AUTH_PASS;
			}
		}
	} else if (cs->auth_state == CON_AUTH_PASS) {
		if (strncmp(ee_config.password, (char *)cs->cmd_buf, EE_PASSWORD_LEN) == 0) {
			cs->auth_state = CON_AUTH_OK;
		} else {
			cs->auth_state = CON_AUTH_USER;
		}
	}*/
}

static void console_cmd_process(struct console_session_t *cs)
{
	struct console_command_t *cmd = NULL;
	char *args[CONSOLE_CMD_ARGS];
	uint8_t i = 1;

	args[0] = cs->cmd_buf;
	while ((args[i] = strstr(args[i-1]+1, " "))) {
		// delete space
		args[i][0] = 0;
		// advance away from (deleted) space
		args[i]++;
		i++;
		if (i >= CONSOLE_CMD_ARGS)
			break;
	}

	cmd = console_cmd_find(cs, args[0], strlen(args[0]));
	if (cmd) {
		cmd->fnc(cs, &args[1]);
	}
}

static uint8_t cmd_help(struct console_session_t *cs, char **args)
{
	struct console_command_t *cmd = &console_command[0];
	char out[CONSOLE_CMD_OUTBUF];
	uint8_t ret = 1;
	uint32_t len;

	if (args[0] == NULL) {
		while (cmd != NULL) {
			len = tfp_sprintf(out, "%s - %s\n", cmd->cmd, cmd->help_short);
			console_session_output(cs, out, len);
			cmd = cmd->next;
		}
		ret = 0;
	} else {
		cmd = console_cmd_find(cs, args[0], strlen(args[0]));
		if (cmd) {
			len = tfp_sprintf(out, "%s - %s\n%s\n", cmd->cmd, cmd->help_short, cmd->help_long);
			ret = 0;
		} else {
			len = tfp_sprintf(out, "Help '%s' error\n", args[0]);
		}
		console_session_output(cs, out, len);
	}
	return ret;
}

static uint8_t cmd_verbose(struct console_session_t *cs, char **args)
{
	char out[CONSOLE_CMD_OUTBUF];
	uint32_t len;

	if (args[0] != NULL) {
		uint32_t verbosity = atoi(args[0]);
		cs->verbosity = (verbosity >= CON_LAST) ? CON_LAST-1 : verbosity;
	}

	len = tfp_sprintf(out, "Verbosity level %d \n", cs->verbosity);
	console_session_output(cs, out, len);

	return 0;
}

static uint8_t cmd_reboot(struct console_session_t *cs, char **args)
{
	NVIC_SystemReset();

	return 0;
}


static uint8_t cmd_dfu(struct console_session_t *cs, char **args)
{
	struct flash_ob_t ob;

	flash_ob_read(&ob);
	ob.user = 0x77;
	flash_ob_write(&ob);

	DCD_DevDisconnect(&USB_Device_dev);
	NVIC_SystemReset();

	return 0;
}

#ifndef FLASH_h_
#define FLASH_h_

#include "platform.h"

struct flash_ob_t {
	uint32_t wrpr;
	uint8_t rdp;
	uint8_t user;
	uint8_t data0;
	uint8_t data1;
};

void flash_ob_read(struct flash_ob_t *ob);
void flash_ob_write(struct flash_ob_t *ob);

#endif

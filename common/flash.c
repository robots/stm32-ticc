
#include "flash.h"

void flash_ob_read(struct flash_ob_t *ob)
{
	ob->wrpr = FLASH->WRPR;
	ob->rdp = 0xAA;
	if (((FLASH->OBR >> 1) & 0x3) == 1) {
		ob->rdp = 0xBB;
	}/*else if (((FLASH->OBR >> 1) & 0x3) == 3) {
		ob->rdp = 0xCC; // USE WITH CAUTION
	}*/
	ob->user = (FLASH->OBR >> 8) & 0xff;
	ob->data0 = (FLASH->OBR >> 16) & 0xff;
	ob->data1 = (FLASH->OBR >> 24) & 0xff;
}

void flash_ob_write(struct flash_ob_t *ob)
{
	FLASH_Unlock();
	FLASH_OB_Unlock();

	FLASH_OB_Erase();

	FLASH_OB_EnableWRP(ob->wrpr);
	FLASH_OB_RDPConfig(ob->rdp);
	FLASH_OB_WriteUser(ob->user);

	// TODO: restore data0 data1
	
	FLASH_OB_Lock();
	FLASH_Lock();

	FLASH_OB_Launch();
}


dfu-util -s 0x08000000 -d 0x0483:0xdf11 -a 0 -D ./main/FLASH_RUN/usbstick_v1/usbstick_v1.bin
dfu-util -s 0x1ffff800 -d 0x0483:0xdf11 -a 1 -D ./bin/option.bin



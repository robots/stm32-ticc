# WinARM template makefile 
# by Martin Thomas, Kaiserslautern, Germany 
# <eversmith(at)heizung-thomas(dot)de>
#
# Released to the Public Domain
# Please read the make user manual!
#
# The user-configuration is based on the WinAVR makefile-template
# written by Eric B. Weddington, Jörg Wunsch, et al. but internal
# handling used here is very different.
# This makefile can also be used with the GNU tools included in
# Yagarto, GNUARM or the codesourcery packages. It should work
# on Unix/Linux-Systems too. Just a rather up-to-date GNU make is
# needed.
#
# MD:
# - removed unused stuff to simplify maintnance 

TCHAIN_PREFIX = arm-none-eabi-

# MCU name, submodel and board
# - MCU used for compiler-option (-mcpu)
# - SUBMDL used for linker-script name (-T) and passed as define
MCU      = cortex-m0
SYSCLOCK_CL = SYSCLK_FREQ_72MHz=72000000

M    ?= usbstick_v1

ifeq ($(M), usbstick_v1)
BOARD = usbstick_v1
TARGET = usbstick_v1
CHIP     = STM32F042
F_XTAL   = 48000000
RAMSIZE  = 0x1800
LINKERSCRIPTPATH = .
endif

# *** This example only supports "FLASH_RUN" ***
# RUN_MODE is passed as define and used for the linker-script filename,
# the user has to implement the necessary operations for 
# the used mode(s) (i.e. no copy of .data, remapping)
# Create FLASH-Image
RUN_MODE=FLASH_RUN
# Create RAM-Image
#RUN_MODE=RAM_RUN

# Exception-vectors placement option is just passed as define,
# the user has to implement the necessary operations (i.e. remapping)
# Exception vectors in FLASH:
VECTOR_TABLE_LOCATION=VECT_TAB_FLASH
#VECTOR_TABLE_LOCATION=VECT_TAB_RAM

# Directory for output files (lst, obj, dep, elf, sym, map, hex, bin etc.)
OUTDIR = $(RUN_MODE)/$(TARGET)

# Target file name (without extension).
# TARGET = $(OUT_FILE)

# Pathes to libraries
APPLIBDIR = ../lib

TPRINTFDIR = $(APPLIBDIR)/tinyprintf
STMSPDDIR = $(APPLIBDIR)/STM32F0xx_StdPeriph_Driver
STMSPDSRCDIR = $(STMSPDDIR)/src
STMSPDINCDIR = $(STMSPDDIR)/inc
STMUSBDRVDIR = $(APPLIBDIR)/STM32_USB_Device_Driver
STMUSBDRVSRCDIR = $(STMUSBDRVDIR)/src
STMUSBDRVINCDIR = $(STMUSBDRVDIR)/inc
STMUSBLIBCOREDIR = $(APPLIBDIR)/STM32_USB_Device_Library/Core
STMUSBLIBCORESRCDIR = $(STMUSBLIBCOREDIR)/src
STMUSBLIBCOREINCDIR = $(STMUSBLIBCOREDIR)/inc
STMUSBLIBCDCDIR = $(APPLIBDIR)/STM32_USB_Device_Library/Class/cdc
STMUSBLIBCDCSRCDIR = $(STMUSBLIBCDCDIR)/src
STMUSBLIBCDCINCDIR = $(STMUSBLIBCDCDIR)/inc
CMSISDIR  = $(APPLIBDIR)/CMSIS
CMSISSTDIR  = $(CMSISDIR)/Device/ST/STM32F0xx
COMMONSRCDIR = ../common
PTDIR = $(APPLIBDIR)/pt


BSP = ../bsp/$(BOARD)

# List C source files here. (C dependencies are automatically generated.)
# use file-extension c for "c-only"-files

## Application:
SRC = main.c system_stm32f0xx.c
SRC += usb_bsp.c usbd_cdc_vcp.c usbd_desc.c usbd_pwr.c usbd_usr.c
SRC += ticc.c tdc7200.c cfg.c


SRC += $(TPRINTFDIR)/printf.c

SRC += $(COMMONSRCDIR)/console.c
#SRC += $(COMMONSRCDIR)/console_dmesg.c
#SRC += $(COMMONSRCDIR)/console_thread.c
#SRC += $(COMMONSRCDIR)/console_telnet.c
#SRC += $(COMMONSRCDIR)/console_uart.c

#SRC += $(COMMONSRCDIR)/crc32.c
SRC += $(COMMONSRCDIR)/gpio.c
SRC += $(COMMONSRCDIR)/exti.c
#SRC += $(COMMONSRCDIR)/fifo.c
SRC += $(COMMONSRCDIR)/flash.c
SRC += $(COMMONSRCDIR)/led.c
#SRC += $(COMMONSRCDIR)/ee_config.c
#SRC += $(COMMONSRCDIR)/eeprom.c
SRC += $(COMMONSRCDIR)/spi.c
SRC += $(COMMONSRCDIR)/systime.c
#SRC += $(COMMONSRCDIR)/netconf.c
#SRC += $(COMMONSRCDIR)/timer.c
#SRC += $(COMMONSRCDIR)/random.c

#SRC += $(BSP)/bsp_console_uart.c
#SRC += $(BSP)/bsp_eth.c
SRC += $(BSP)/bsp_led.c
SRC += $(BSP)/bsp_spi.c

## compiler-specific sources
SRC += $(APPLIBDIR)/gcc/startup_stm32f042.c

## CMSIS for STM32
#SRC += $(CMSISDIR)/core_cm3.c
#SRC += $(CMSISSTDIR)/Source/Templates/system_stm32f0xx.c
#SRC += $(APPLIBDIR)/gcc/fault_handlers.c

## used parts of the STM-Library
#SRC += $(STMSPDSRCDIR)/stm32f0xx_adc.c
#SRC += $(STMSPDSRCDIR)/stm32f0xx_can.c
#SRC += $(STMSPDSRCDIR)/stm32f0xx_cec.c
#SRC += $(STMSPDSRCDIR)/stm32f0xx_comp.c
#SRC += $(STMSPDSRCDIR)/stm32f0xx_crc.c
SRC += $(STMSPDSRCDIR)/stm32f0xx_crs.c
#SRC += $(STMSPDSRCDIR)/stm32f0xx_dac.c
#SRC += $(STMSPDSRCDIR)/stm32f0xx_dbgmcu.c
SRC += $(STMSPDSRCDIR)/stm32f0xx_dma.c
SRC += $(STMSPDSRCDIR)/stm32f0xx_exti.c
SRC += $(STMSPDSRCDIR)/stm32f0xx_flash.c
SRC += $(STMSPDSRCDIR)/stm32f0xx_gpio.c
#SRC += $(STMSPDSRCDIR)/stm32f0xx_i2c.c
#SRC += $(STMSPDSRCDIR)/stm32f0xx_iwdg.c
SRC += $(STMSPDSRCDIR)/stm32f0xx_misc.c
#SRC += $(STMSPDSRCDIR)/stm32f0xx_pwr.c
SRC += $(STMSPDSRCDIR)/stm32f0xx_rcc.c
#SRC += $(STMSPDSRCDIR)/stm32f0xx_rtc.c
SRC += $(STMSPDSRCDIR)/stm32f0xx_spi.c
SRC += $(STMSPDSRCDIR)/stm32f0xx_syscfg.c
#SRC += $(STMSPDSRCDIR)/stm32f0xx_tim.c
#SRC += $(STMSPDSRCDIR)/stm32f0xx_usart.c
#SRC += $(STMSPDSRCDIR)/stm32f0xx_wwdg.c

## usb driver
#SRC += $(STMUSBDRVSRCDIR)/usb_bsp_template.c
SRC += $(STMUSBDRVSRCDIR)/usb_core.c
SRC += $(STMUSBDRVSRCDIR)/usb_dcd.c
SRC += $(STMUSBDRVSRCDIR)/usb_dcd_int.c

## usb library
SRC += $(STMUSBLIBCORESRCDIR)/usbd_core.c
SRC += $(STMUSBLIBCORESRCDIR)/usbd_ioreq.c
SRC += $(STMUSBLIBCORESRCDIR)/usbd_req.c

SRC += $(STMUSBLIBCDCSRCDIR)/usbd_cdc_core.c

# List C source files here which must be compiled in ARM-Mode (no -mthumb).
# use file-extension c for "c-only"-files
## just for testing, timer.c could be compiled in thumb-mode too
SRCARM = 

# List C++ source files here.
# use file-extension .cpp for C++-files (not .C)
CPPSRC = 

# List C++ source files here which must be compiled in ARM-Mode.
# use file-extension .cpp for C++-files (not .C)
CPPSRCARM = 

# List Assembler source files here.
# Make them always end in a capital .S. Files ending in a lowercase .s
# will not be considered source files but generated files (assembler
# output from the compiler), and will be deleted upon "make clean"!
# Even though the DOS/Win* filesystem matches both .s and .S the same,
# it will preserve the spelling of the filenames, and gcc itself does
# care about how the name is spelled on its command-line.
ASRC = 

# List Assembler source files here which must be assembled in ARM-Mode..
ASRCARM = 

# List any extra directories to look for include files here.
#    Each directory must be seperated by a space.
EXTRAINCDIRS  = $(STMSPDINCDIR) $(CMSISDIR)/Include $(CMSISSTDIR)/Include
EXTRAINCDIRS += $(TPRINTFDIR)
EXTRAINCDIRS += $(STMUSBDRVINCDIR)
EXTRAINCDIRS += $(STMUSBLIBCOREINCDIR)
EXTRAINCDIRS += $(STMUSBLIBCDCINCDIR)
EXTRAINCDIRS += ../common
EXTRAINCDIRS += ../bsp

# List any extra directories to look for library files here.
# Also add directories where the linker should search for
# includes from linker-script to the list
#     Each directory must be seperated by a space.
EXTRA_LIBDIRS =

# Extra libraries
#    Each library-name must be seperated by a space.
#    i.e. to link with libxyz.a, libabc.a and libefsl.a: 
#    EXTRA_LIBS = xyz abc efsl
# for newlib-lpc (file: libnewlibc-lpc.a):
#    EXTRA_LIBS = newlib-lpc
EXTRA_LIBS =


# Optimization level, can be [0, 1, 2, 3, s]. 
# 0 = turn off optimization. s = optimize for size.
# (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
OPT = s
#OPT = 2
#OPT = 3
#OPT = 0

# Output format. (can be ihex or binary or both)
#  binary to create a load-image in raw-binary format i.e. for SAM-BA, 
#  ihex to create a load-image in Intel hex format i.e. for lpc21isp
#LOADFORMAT = ihex
#LOADFORMAT = binary
#LOADFORMAT = both

# Debugging format.
DEBUG = dwarf-2

# Place project-specific -D (define) and/or 
# -U options for C here.
CDEFS = 
CDEFS += -DRAMSIZE=$(RAMSIZE)
#CDEFS += -DHSE_VALUE=$(F_XTAL)UL
CDEFS += -D$(SYSCLOCK_CL)
CDEFS += -D$(BOARD)
#CDEFS += -DLWIP_DEBUG
CDEFS += -DUSE_STDPERIPH_DRIVER
#CDEFS += -DSTM32_SD_USE_DMA
CDEFS += -DSTARTUP_DELAY
# enable modifications in STM's libraries
CDEFS += -DMOD_MTHOMAS_STMLIB
# enable modifications in ChaN's FAT-module
##CDEFS += -DMOD_MTHOMAS_FFAT
# enable parameter-checking in STM's library
#CDEFS += -DUSE_FULL_ASSERT

# Place project-specific -D and/or -U options for 
# Assembler with preprocessor here.
ADEFS = -D__ASSEMBLY__

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=gnu99

#-----

ifdef VECTOR_TABLE_LOCATION
CDEFS += -D$(VECTOR_TABLE_LOCATION)
ADEFS += -D$(VECTOR_TABLE_LOCATION)
endif

CDEFS += -D$(RUN_MODE) -D$(CHIP)
#CDEFS += -D$(RUN_MODE)
#CDEFS += -DSW_VER=$$(cat ../.version)
#CDEFS += -DSW_BUILD=$$(cat ../.build)
ADEFS += -D$(RUN_MODE) -D$(CHIP)

THUMB    = -mthumb


# Compiler flags.
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
#
# Flags for C and C++ (arm-elf-gcc/arm-elf-g++)
CFLAGS =  -g$(DEBUG)
CFLAGS += -O$(OPT)
CFLAGS += -mcpu=$(MCU) $(THUMB_IW) -msoft-float 
CFLAGS += $(CDEFS)
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS)) -I.
# when using ".ramfunc"s without longcall:
##CFLAGS += -mlong-calls
# -mapcs-frame is important if gcc's interrupt attributes are used
# (at least from my eabi tests), not needed if assembler-wrapper is used 
##CFLAGS += -mapcs-frame 
##CFLAGS += -fomit-frame-pointer
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -mfix-cortex-m3-ldrd
CFLAGS += -Wall -Wextra
CFLAGS += -Wimplicit -Wcast-align -Wpointer-arith
CFLAGS += -Wredundant-decls -Wshadow
#CFLAGS += -Wcast-qual
CFLAGS += -Wcast-align
#CFLAGS += -pedantic
CFLAGS += -Wa,-adhlns=$(addprefix $(OUTDIR)/, $(notdir $(addsuffix .lst, $(basename $<))))
# Compiler flags to generate dependency files:
CFLAGS += -MD -MP -MF $(OUTDIR)/dep/$(@F).d

# flags only for C
CONLYFLAGS += -Wnested-externs 
CONLYFLAGS += $(CSTANDARD)

# flags only for C++ (arm-elf-g++)
CPPFLAGS = -fno-rtti -fno-exceptions
CPPFLAGS = 

# Assembler flags.
#  -Wa,...:    tell GCC to pass this to the assembler.
#  -ahlns:     create listing
#  -g$(DEBUG): have the assembler create line number information
ASFLAGS  = -mcpu=$(MCU) $(THUMB_IW) -I. -x assembler-with-cpp
ASFLAGS += $(ADEFS)
ASFLASG += -ahls -mapcs-32
ASFLAGS += -Wa,-adhlns=$(addprefix $(OUTDIR)/, $(notdir $(addsuffix .lst, $(basename $<))))
ASFLAGS += -Wa,-g$(DEBUG)
ASFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))

MATH_LIB = -lm

# Link with the GNU C++ stdlib.
#CPLUSPLUS_LIB = -lstdc++

# Linker flags.
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -nostartfiles -Wl,-Map=$(OUTDIR)/$(TARGET).map,--cref,--gc-sections
LDFLAGS += $(patsubst %,-L%,$(EXTRA_LIBDIRS))
LDFLAGS += $(patsubst %,-l%,$(EXTRA_LIBS))
LDFLAGS += $(MATH_LIB)
LDFLAGS += $(CPLUSPLUS_LIB)
LDFLAGS += -lc -lgcc 

# Set linker-script name depending on selected run-mode and submodel name
ifeq ($(RUN_MODE),RAM_RUN)
LDFLAGS +=-T$(LINKERSCRIPTPATH)/$(CHIP)_ram.ld
else 
#LDFLAGS +=-T$(LINKERSCRIPTPATH)/$(CHIP)_freertos_flash.ld
LDFLAGS +=-T$(LINKERSCRIPTPATH)/$(CHIP)_flash.ld
endif


# Define programs and commands.
CC      = $(TCHAIN_PREFIX)gcc
CPP     = $(TCHAIN_PREFIX)g++
AR      = $(TCHAIN_PREFIX)ar
OBJCOPY = $(TCHAIN_PREFIX)objcopy
OBJDUMP = $(TCHAIN_PREFIX)objdump
SIZE    = $(TCHAIN_PREFIX)size
NM      = $(TCHAIN_PREFIX)nm

# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = "-------- begin (mode: $(RUN_MODE)) --------"
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before: 
MSG_SIZE_AFTER = Size after build:
MSG_LOAD_FILE = Creating load file:
MSG_EXTENDED_LISTING = Creating Extended Listing/Disassembly:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = "**** Linking :"
MSG_COMPILING = "**** Compiling C :"
MSG_COMPILING_ARM = "**** Compiling C (ARM-only):"
MSG_COMPILINGCPP = "Compiling C++ :"
MSG_COMPILINGCPP_ARM = "Compiling C++ (ARM-only):"
MSG_ASSEMBLING = "**** Assembling:"
MSG_ASSEMBLING_ARM = "****Assembling (ARM-only):"
MSG_CLEANING = Cleaning project:
MSG_FORMATERROR = Can not handle output-format
MSG_LPC21_RESETREMINDER = You may have to bring the target in bootloader-mode now.
MSG_ASMFROMC = "Creating asm-File from C-Source:"
MSG_ASMFROMC_ARM = "Creating asm-File from C-Source (ARM-only):"

# List of all source files.
ALLSRC     = $(ASRCARM) $(ASRC) $(SRCARM) $(SRC) $(CPPSRCARM) $(CPPSRC)
# List of all source files without directory and file-extension.
ALLSRCBASE = $(notdir $(basename $(ALLSRC)))

# Define all object files.
ALLOBJ     = $(addprefix $(OUTDIR)/, $(addsuffix .o, $(ALLSRCBASE)))

# Define all listing files (used for make clean).
LSTFILES   = $(addprefix $(OUTDIR)/, $(addsuffix .lst, $(ALLSRCBASE)))
# Define all depedency-files (used for make clean).
DEPFILES   = $(addprefix $(OUTDIR)/dep/, $(addsuffix .o.d, $(ALLSRCBASE)))

# Default target.
all: begin gccversion build sizeafter finished end

elf: $(OUTDIR)/$(TARGET).elf
lss: $(OUTDIR)/$(TARGET).lss 
sym: $(OUTDIR)/$(TARGET).sym
hex: $(OUTDIR)/$(TARGET).hex
bin: $(OUTDIR)/$(TARGET).bin

ifeq ($(LOADFORMAT),ihex)
build: elf hex lss sym
else 
ifeq ($(LOADFORMAT),binary)
build: elf bin lss sym
else 
ifeq ($(LOADFORMAT),both)
build: elf hex bin lss sym
else 
build: elf lss sym
endif
endif
endif


# Eye candy.
begin:
##	@echo
	@echo $(MSG_BEGIN)

finished:
##	@echo $(MSG_ERRORS_NONE)

end:
	@echo $(MSG_END)
##	@echo

# Display sizes of sections.
ELFSIZE = $(SIZE) -A -x $(OUTDIR)/$(TARGET).elf

sizeafter:
	@echo $(MSG_SIZE_AFTER)
	$(ELFSIZE)
	
# Display compiler version information.
gccversion : 
	@$(CC) --version
#	@echo $(ALLOBJ)

flash_dump:
	@echo "Dumping flash"
	openocd -f openocd.cfg -c "init" -c "halt" -c "flash probe 0" -c "dump_image flash.img 0x08000000 0x20000" -c "reset run" -c "exit"

flash_load: bin
#	@echo "Programming with OPENOCD"
#	openocd -f openocd.cfg -c "init" -c "halt" -c "stm32x mass_erase 0" -c "flash write_image $(OUTDIR)/$(TARGET).elf elf" -c "soft_reset_halt" -c "reset run" -c "exit"
	../dfuse-dfu-util/src/dfu-util --dfuse 0x8002000 -D $(OUTDIR)/stm32_ises_blue_usb.bin -d 0x0483:0xdf11 -R



# Create final output file (.hex) from ELF output file.
%.hex: %.elf
##	@echo
	@echo $(MSG_LOAD_FILE) $@
	$(OBJCOPY) -O ihex $< $@
	
# Create final output file (.bin) from ELF output file.
%.bin: %.elf
##	@echo
	@echo $(MSG_LOAD_FILE) $@
	$(OBJCOPY) -O binary $< $@

# Create extended listing file/disassambly from ELF output file.
# using objdump testing: option -C
%.lss: %.elf
##	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S -C -r $< > $@
#	$(OBJDUMP) -x -S $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
##	@echo
	@echo $(MSG_SYMBOL_TABLE) $@
	$(NM) -n $< > $@

# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(ALLOBJ)
%.elf:  $(ALLOBJ)
	@echo
	@echo $(MSG_LINKING) $@ && $(CC) $(THUMB) $(CFLAGS) $(ALLOBJ) --output $@ $(LDFLAGS)
# use $(CC) for C-only projects or $(CPP) for C++-projects:
#	$(CPP) $(THUMB) $(CFLAGS) $(ALLOBJ) --output $@ $(LDFLAGS)


# Assemble: create object files from assembler source files.
define ASSEMBLE_TEMPLATE
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
##	@echo
	@echo $(MSG_ASSEMBLING) $$< "->" $$@ && $(CC) -c $(THUMB) $$(ASFLAGS) $$< -o $$@ 
endef
$(foreach src, $(ASRC), $(eval $(call ASSEMBLE_TEMPLATE, $(src)))) 

# Assemble: create object files from assembler source files. ARM-only
define ASSEMBLE_ARM_TEMPLATE
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
##	@echo
	@echo $(MSG_ASSEMBLING_ARM) $$< "->" $$@ && $(CC) -c $$(ASFLAGS) $$< -o $$@ 
endef
$(foreach src, $(ASRCARM), $(eval $(call ASSEMBLE_ARM_TEMPLATE, $(src)))) 


# Compile: create object files from C source files.
define COMPILE_C_TEMPLATE
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
##	@echo
	@echo $(MSG_COMPILING) $$< "->" $$@ && 	$(CC) -c $(THUMB) $$(CFLAGS) $$(CONLYFLAGS) $$< -o $$@ 
endef
$(foreach src, $(SRC), $(eval $(call COMPILE_C_TEMPLATE, $(src)))) 

# Compile: create object files from C source files. ARM-only
define COMPILE_C_ARM_TEMPLATE
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
##	@echo
	@echo $(MSG_COMPILING_ARM) $$< "->" $$@ && $(CC) -c $$(CFLAGS) $$(CONLYFLAGS) $$< -o $$@ 
endef
$(foreach src, $(SRCARM), $(eval $(call COMPILE_C_ARM_TEMPLATE, $(src)))) 


# Compile: create object files from C++ source files.
define COMPILE_CPP_TEMPLATE
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
##	@echo
	@echo $(MSG_COMPILINGCPP) $$< "->" $$@ && $(CC) -c $(THUMB) $$(CFLAGS) $$(CPPFLAGS) $$< -o $$@ 
endef
$(foreach src, $(CPPSRC), $(eval $(call COMPILE_CPP_TEMPLATE, $(src)))) 

# Compile: create object files from C++ source files. ARM-only
define COMPILE_CPP_ARM_TEMPLATE
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
##	@echo
	@echo $(MSG_COMPILINGCPP_ARM) $$< "->" $$@ && $(CC) -c $$(CFLAGS) $$(CPPFLAGS) $$< -o $$@ 
endef
$(foreach src, $(CPPSRCARM), $(eval $(call COMPILE_CPP_ARM_TEMPLATE, $(src)))) 


# Compile: create assembler files from C source files. ARM/Thumb
$(SRC:.c=.s) : %.s : %.c
	@echo $(MSG_ASMFROMC) $< to $@
	$(CC) $(THUMB) -S $(CFLAGS) $(CONLYFLAGS) $< -o $@

# Compile: create assembler files from C source files. ARM only
$(SRCARM:.c=.s) : %.s : %.c
	@echo $(MSG_ASMFROMC_ARM) $< to $@
	$(CC) -S $(CFLAGS) $(CONLYFLAGS) $< -o $@

# Target: clean project.
clean: begin clean_list finished end

clean_list :
##	@echo
	@echo $(MSG_CLEANING)
	rm -f $(OUTDIR)/$(TARGET).map
	rm -f $(OUTDIR)/$(TARGET).elf
	rm -f $(OUTDIR)/$(TARGET).hex
	rm -f $(OUTDIR)/$(TARGET).bin
	rm -f $(OUTDIR)/$(TARGET).sym
	rm -f $(OUTDIR)/$(TARGET).lss
	rm -f $(ALLOBJ)
	rm -f $(LSTFILES)
	rm -f $(DEPFILES)
	rm -f $(SRC:.c=.s)
	rm -f $(SRCARM:.c=.s)
	rm -f $(CPPSRC:.cpp=.s)
	rm -f $(CPPSRCARM:.cpp=.s)


# Create output files directory
# all known MS Windows OS define the ComSpec environment variable
$(shell mkdir -p $(OUTDIR) 2>/dev/null)

# Include the dependency files.
-include $(shell mkdir -p $(OUTDIR) 2>/dev/null) $(shell mkdir -p $(OUTDIR)/dep 2>/dev/null) $(wildcard $(OUTDIR)/dep/*)


# Listing of phony targets.
.PHONY : all begin finish end sizebefore sizeafter gccversion \
build elf hex bin lss sym clean clean_list program


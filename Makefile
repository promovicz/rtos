
all: current


current_makefile := $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

DEFAULT_SOURCE=$(shell dirname $(call current_makefile))

#### TARGET SELECTION ####

APP ?= cli
ENV ?= booted
BOARD ?= logomatic

CORE_OBJ = \
	core/tty.o \
	core/parse.o \
	core/system.o \
	core/cli.o \
	core/device.o \
	core/timer.o \
	core/clock.o \
	core/file.o \
	core/irq.o \
	core/memory.o \
	serial_fifo.o \
# core/filesystem.o

CLI_OBJ=commands/gpio.o commands/mem.o commands/sys.o commands/nmea.o commands/posix.o commands/gps.o commands/lpc.o
SENSOR_OBJ=sensor/scp.o sensor/nmea.o

DISARM_OBJ=pkg/libdisarm/src/libdisarm/args.o pkg/libdisarm/src/libdisarm/print.o pkg/libdisarm/src/libdisarm/parser.o
RRFAT_OBJ=fat/fat16.o fat/partition.o fat/rootdir.o fat/sd_raw.o

POSIX_OBJ=posix/process.o posix/epoll.o posix/file_console.o posix/signal.o posix/control.o
POSIX_SYS_OBJ=posix/sys_errno.o posix/sys_errlist.o posix/sys_file.o posix/sys_sleep.o posix/sys_mcontext.o posix/sys_ucontext.o posix/sys_sysconf.o posix/sys_mmap.o posix/sys_munmap.o posix/sys_mremap.o posix/sys_getpagesize.o

RTOS_OBJECTS=$(CORE_OBJ) $(CLI_OBJ) $(SENSOR_OBJ) $(POSIX_OBJ) $(RRFAT_OBJ)
ifneq ($(BOARD),emulator)
RTOS_OBJECTS+=$(DISARM_OBJ) $(POSIX_SYS_OBJ)
RTOS_LIBRARIES=dietlibc.a
endif


#### EMULATOR ####

BOARD_emulator_PLATFORM=emulator
BOARD_emulator_OBJECTS=\
	platform/emulator/board.o

PLATFORM_emulator_CROSS=

#### BOARD logomatic ####

BOARD_logomatic_PLATFORM=lpc2148
BOARD_logomatic_OBJECTS=\
	board/logomatic/board.o \
	board/logomatic/microsd.o

#### PLATFORM lpc21 ####

PLATFORM_lpc21_LIBRARIES=\
	lpcusb.a
PLATFORM_lpc21_OBJECTS=\
	platform/lpc21/irq.o \
	platform/lpc21/scb.o \
	platform/lpc21/pll.o \
	platform/lpc21/mam.o \
	platform/lpc21/vpb.o \
	platform/lpc21/vic.o \
	platform/lpc21/pcon.o \
	platform/lpc21/reset.o \
	platform/lpc21/pin.o \
	platform/lpc21/pinsel.o \
	platform/lpc21/timer.o \
	platform/lpc21/eint.o \
	platform/lpc21/gpio.o \
	platform/lpc21/uart.o \
	platform/lpc21/rtc.o \
	platform/lpc21/ssp.o \
	platform/lpc21/spi.o \
	platform/lpc21/wdt.o \
	platform/lpc21/device.o \
	platform/lpc21/stubs.o \
	platform/lpc21/vcom.o

PLATFORM_lpc2148_CROSS=arm-elf-
PLATFORM_lpc2148_DEFINES=-DMCU_LPC2148
PLATFORM_lpc2148_LIBRARIES=\
	$(PLATFORM_lpc21_LIBRARIES)
PLATFORM_lpc2148_OBJECTS=\
	$(PLATFORM_lpc21_OBJECTS) \
	platform/lpc21/mcu-lpc2148.o

#### ####

APP_cli_OBJECTS=\
	main.o

#### ESSENTIAL DIRECTORIES ####

SOURCE?=$(DEFAULT_SOURCE)

#### TOOLCHAIN SELECTION ####

CC=$(PLATFORM_$(PLATFORM)_CROSS)gcc
NM=$(PLATFORM_$(PLATFORM)_CROSS)nm
SIZE=$(PLATFORM_$(PLATFORM)_CROSS)size
OBJCOPY=$(PLATFORM_$(PLATFORM)_CROSS)objcopy
OBJDUMP=$(PLATFORM_$(PLATFORM)_CROSS)objdump

HOSTCC?=gcc

#### TOOLCHAIN CONFIGURATION ####



#### GENERIC RULES ####

%.p: %.c
	@mkdir -p `dirname $(*)`
	@$(CC) $(CFLAGS) -M -o $(*).d $(<)
	@sed 's|.*\.o:|$(@:.p=.o): |g' < $*.d > $@; rm -f $*.d; [ -s $@ ] || rm -f $@

%.p: %.S
	@mkdir -p `dirname $(*)`
	@$(CC) $(ASFLAGS) -M -o $(*).d $(<)
	@sed 's|.*\.o:|$(@:.p=.o): |g' < $*.d > $@; rm -f $*.d; [ -s $@ ] || rm -f $@

%.o: %.c
	@mkdir -p `dirname $(*)`
	$(CC) $(CFLAGS) -Wa,-adhlns=$(@:.o=.lst) -c -o $@ $<

%.o: %.S
	@mkdir -p `dirname $(*)`
	$(CC) $(ASFLAGS) -Wa,-adhlns=$(@:.o=.lst) -c -o $@ $<

%.bin: %.elf
	$(OBJCOPY) --gap-fill=0xff -O binary $^ $@

%.hex: %.elf
	$(OBJCOPY) --gap-fill=0xff -O ihex $^ $@

%.lst: %.elf
	$(OBJDUMP) -d $^ > $@

%.size: %.elf
	$(SIZE) $^ | tee $@

%.syms: %.elf
	$(NM) -f bsd -alnS $^ > $@





dietlibc.a dietlibm.a:
	export CROSS=$(PLATFORM_$(PLATFORM)_CROSS) && cd $(SOURCE)/pkg/dietlibc && make clean && make all
	cp $(SOURCE)/pkg/dietlibc/bin-arm/dietlibc.a dietlibc.a
	cp $(SOURCE)/pkg/dietlibc/bin-arm/libm.a dietlibm.a

lpcusb.a:
	export CROSS=$(PLATFORM_$(PLATFORM)_CROSS) && cd $(SOURCE)/pkg/lpcusb/target && make clean && make depend && make lib
	cp $(SOURCE)/pkg/lpcusb/target/usbstack.a lpcusb.a



#### ####

CFLAGS_WARNINGS=-Wall -Wextra -Wshadow -Wpointer-arith -Wno-cast-align -Wimplicit -Wno-unused -Wredundant-decls -Wnested-externs -Wbad-function-cast -Wsign-compare -Waggregate-return -Wno-format-extra-args

ifneq ($(BOARD),emulator)

LDSCRIPT=board/$(BOARD)/$(ENV).lds
LDSFLAG=-T $(SOURCE)/$(LDSCRIPT)
CRT=board/$(BOARD)/$(ENV).o

CFLAGS_CONFIG=-gdwarf-2 -Os -ffunction-sections -nostdlib
CFLAGS_BUILTIN=-fno-builtin-execl -fno-builtin-execle

CFLAGS_TARGET=-mcpu=arm7tdmi -mfloat-abi=soft -marm

CFLAGS=$(CFLAGS_CONFIG) $(CFLAGS_TARGET) $(CFLAGS_BUILTIN) $(CFLAGS_WARNINGS) $(INCLUDEFLAGS) $(DEFINE)
ASFLAGS=-D__ASSEMBLY__ $(DEFINE) $(INCLUDEFLAGS) $(CFLAGS_TARGET)
LDFLAGS=-static -static-libgcc -nostartfiles -nostdlib -nodefaultlibs -Wl,--gc-sections -Wl,--cref

INCLUDEFLAGS=-I$(SOURCE)/include -I$(SOURCE)/pkg/lpcusb/target -I$(SOURCE)/pkg/dietlibc/include -I$(SOURCE)/pkg/libdisarm/src -I$(SOURCE)
DEFINE=-DLPC214x -DNDEBUG -DLPC_MEMMAP=MEM_MAP_USER_RAM \
	$(PLATFORM_$(PLATFORM)_DEFINES) \
	$(BOARD_$(BOARD)_DEFINES)

else

INCLUDEFLAGS=-I$(SOURCE)/include -I$(SOURCE)

CFLAGS_CONFIG=-gdwarf-2 -Os -ffunction-sections

CFLAGS=$(CFLAGS_CONFIG) $(CFLAGS_WARNINGS) $(INCLUDEFLAGS)
LDFLAGS=-state -static-libgcc

endif

PLATFORM=$(BOARD_$(BOARD)_PLATFORM)

CURRENT_ELF=$(BOARD)-$(APP)-$(ENV).elf
CURRENT_MAP=$(BOARD)-$(APP)-$(ENV).map
CURRENT_BIN=$(BOARD)-$(APP)-$(ENV).bin
CURRENT_HEX=$(BOARD)-$(APP)-$(ENV).hex
CURRENT_LST=$(BOARD)-$(APP)-$(ENV).lst
CURRENT_SIZE=$(BOARD)-$(APP)-$(ENV).size
CURRENT_SYMS=$(BOARD)-$(APP)-$(ENV).syms

CURRENT_LIBS=\
	$(RTOS_LIBRARIES) \
	$(PLATFORM_$(PLATFORM)_LIBRARIES) \
	$(BOARD_$(BOARD)_LIBRARIES) \
	$(APP_$(APP)_LIBRARIES

CURRENT_OBJS=\
	$(CRT) \
	$(RTOS_OBJECTS) \
	$(PLATFORM_$(PLATFORM)_OBJECTS) \
	$(BOARD_$(BOARD)_OBJECTS) \
	$(APP_$(APP)_OBJECTS)

CURRENT_DEPS=$(CURRENT_OBJS:.o=.p)

CURRENT_LSTS=$(CURRENT_OBJS:.o=.lst)

VPATH=$(SOURCE)

depend: $(CURRENT_DEPS)

current: depend \
	$(CURRENT_ELF) $(CURRENT_MAP) \
	$(CURRENT_HEX) $(CURRENT_BIN) \
	$(CURRENT_LST) $(CURRENT_SIZE) $(CURRENT_SYMS)

clean:
	rm -rf $(CURRENT_ELF) $(CURRENT_MAP) $(CURRENT_HEX) $(CURRENT_BIN) $(CURRENT_LST) $(CURRENT_SIZE) $(CURRENT_SYMS) \
		$(CURRENT_OBJS) $(CURRENT_LSTS) $(CURRENT_DEPS) $(CURRENT_LIBS)


$(CURRENT_ELF) $(CURRENT_MAP): $(LDSCRIPT) $(CURRENT_OBJS) $(CURRENT_LIBS)
	$(CC) $(LDFLAGS) $(LDSFLAG) \
		-Wl,-Map="$(CURRENT_MAP)" -o $(CURRENT_ELF) \
		-Wl,--start-group -lgcc $(CURRENT_OBJS) $(CURRENT_LIBS) -Wl,--end-group

-include $(CURRENT_DEPS)


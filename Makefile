
all: current


current_makefile := $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

DEFAULT_SOURCE=$(shell dirname $(call current_makefile))

#### TARGET SELECTION ####

APP ?= cli
ENV ?= booted
BOARD ?= logomatic

#### ESSENTIAL DIRECTORIES ####

SOURCE?=$(DEFAULT_SOURCE)

#### TOOLCHAIN SELECTION ####

CROSS?=arm-elf-

CC=$(CROSS)gcc
NM=$(CROSS)nm
SIZE=$(CROSS)size
OBJCOPY=$(CROSS)objcopy
OBJDUMP=$(CROSS)objdump

HOSTCC?=gcc

#### TOOLCHAIN CONFIGURATION ####

CFLAGS_CONFIG=-gdwarf-2 -Os -ffunction-sections -nostdlib
CFLAGS_TARGET=-mcpu=arm7tdmi -mfloat-abi=soft -marm
CFLAGS_BUILTIN=-fno-builtin-execl -fno-builtin-execle
CFLAGS_WARNINGS=-Wall -Wextra -Wshadow -Wpointer-arith -Wno-cast-align -Wimplicit -Wno-unused -Wredundant-decls -Wnested-externs -Wbad-function-cast -Wsign-compare -Waggregate-return -Wno-format-extra-args

CFLAGS=$(CFLAGS_CONFIG) $(CFLAGS_TARGET) $(CFLAGS_BUILTIN) $(CFLAGS_WARNINGS) $(INCLUDEFLAGS) $(DEFINE)
ASFLAGS=-D__ASSEMBLY__ $(DEFINE) $(INCLUDEFLAGS) $(CFLAGS_TARGET)
LDFLAGS=-static -static-libgcc -nostartfiles -nostdlib -nodefaultlibs -Wl,--gc-sections -Wl,--cref

INCLUDEFLAGS=-I$(SOURCE)/include -I$(SOURCE)/pkg/lpcusb/target -I$(SOURCE)/pkg/dietlibc/include -I$(SOURCE)/pkg/libdisarm/src -I$(SOURCE)
DEFINE=-DLPC214x -DNDEBUG -DLPC_MEMMAP=MEM_MAP_USER_RAM




CORE_OBJ=core/tty.o core/parse.o core/system.o core/cli.o core/device.o core/timer.o core/clock.o core/file.o core/irq.o core/memory.o \
	serial_fifo.o vcom.o # core/filesystem.o

CLI_OBJ=commands/gpio.o commands/mem.o commands/sys.o commands/nmea.o commands/posix.o commands/gps.o commands/lpc.o
SENSOR_OBJ=sensor/scp.o sensor/nmea.o

DISARM_OBJ=pkg/libdisarm/src/libdisarm/args.o pkg/libdisarm/src/libdisarm/print.o pkg/libdisarm/src/libdisarm/parser.o
RRFAT_OBJ=fat/fat16.o fat/partition.o fat/rootdir.o fat/sd_raw.o

POSIX_OBJ=posix/process.o posix/epoll.o posix/file_console.o posix/signal.o posix/control.o
POSIX_SYS_OBJ=posix/sys_errno.o posix/sys_errlist.o posix/sys_file.o posix/sys_sleep.o posix/sys_mcontext.o posix/sys_ucontext.o posix/sys_sysconf.o posix/sys_mmap.o posix/sys_munmap.o posix/sys_mremap.o posix/sys_getpagesize.o

COMMON_LIB=lpcusb.a dietlibc.a


COMMON_OBJ=$(CORE_OBJ) $(CLI_OBJ) $(SENSOR_OBJ) $(DISARM_OBJ) $(RRFAT_OBJ) $(POSIX_OBJ) $(POSIX_SYS_OBJ)


MCU_OBJS_lpc2148= \
	lpc/scb.o lpc/pll.o lpc/mam.o lpc/vpb.o lpc/vic.o lpc/pinsel.o \
    lpc/timer.o lpc/gpio.o lpc/rtc.o lpc/ssp.o lpc/spi.o lpc/uart.o \
	lpc/device.o lpc/eint.o lpc/pin.o lpc/pcon.o lpc/wdt.o \
	lpc/reset.o lpc/stubs.o


APP_OBJS_cli=main.o

BOARD_OBJS_logomatic=$(MCU_OBJS_lpc2148) board/logomatic/board.o board/logomatic/microsd.o

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
	export CROSS=$(CROSS) && cd $(SOURCE)/pkg/dietlibc && make clean && make all
	cp $(SOURCE)/pkg/dietlibc/bin-arm/dietlibc.a dietlibc.a
	cp $(SOURCE)/pkg/dietlibc/bin-arm/libm.a dietlibm.a

lpcusb.a:
	export CROSS=$(CROSS) && cd $(SOURCE)/pkg/lpcusb/target && make clean && make depend && make lib
	cp $(SOURCE)/pkg/lpcusb/target/usbstack.a lpcusb.a



#### ####

LDSCRIPT=board/$(BOARD)/$(ENV).lds

CURRENT_CRT=board/$(BOARD)/$(ENV).o

CURRENT_ELF=$(BOARD)-$(APP)-$(ENV).elf
CURRENT_MAP=$(BOARD)-$(APP)-$(ENV).map
CURRENT_BIN=$(BOARD)-$(APP)-$(ENV).bin
CURRENT_HEX=$(BOARD)-$(APP)-$(ENV).hex
CURRENT_LST=$(BOARD)-$(APP)-$(ENV).lst
CURRENT_SIZE=$(BOARD)-$(APP)-$(ENV).size
CURRENT_SYMS=$(BOARD)-$(APP)-$(ENV).syms

CURRENT_LIBS=$(COMMON_LIB)

CURRENT_OBJS=$(CURRENT_CRT) $(COMMON_OBJ) $(APP_OBJS_$(APP)) $(BOARD_OBJS_$(BOARD))

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
	$(CC) $(LDFLAGS) -T $(SOURCE)/$(LDSCRIPT) \
		-Wl,-Map="$(CURRENT_MAP)" -o $(CURRENT_ELF) \
		-Wl,--start-group -lgcc $(CURRENT_OBJS) $(CURRENT_LIBS) -Wl,--end-group

-include $(CURRENT_DEPS)



CROSS?=arm-elf-

CC=$(CROSS)gcc
SIZE=$(CROSS)size
OBJCOPY=$(CROSS)objcopy
OBJDUMP=$(CROSS)objdump

CFLAGS_CONFIG=-gdwarf-2 -Os -ffunction-sections -nostdlib
CFLAGS_TARGET=-mcpu=arm7tdmi -mfloat-abi=soft -marm
CFLAGS_BUILTIN=-fno-builtin-execl -fno-builtin-execle
CFLAGS_WARNINGS=-Wall -Wextra -Wshadow -Wpointer-arith -Wno-cast-align -Wimplicit -Wno-unused -Wredundant-decls -Wnested-externs -Wbad-function-cast -Wsign-compare -Waggregate-return -Wno-format-extra-args

CFLAGS=$(CFLAGS_CONFIG) $(CFLAGS_TARGET) $(CFLAGS_BUILTIN) $(CFLAGS_WARNINGS) $(INCLUDEFLAGS) $(DEFINE)
ASFLAGS=-D__ASSEMBLY__ $(DEFINE) $(INCLUDEFLAGS)
LDFLAGS=-static -static-libgcc -nostartfiles -nostdlib -nodefaultlibs -Wl,--gc-sections -Wl,--cref

LDSCRIPT=board/logomatic/booted.lds

LIBS=pkg/lpcusb/target/usbstack.a pkg/dietlibc/bin-arm/dietlibc.a

LOGOMATICOBJS=board/logomatic/booted.o board/logomatic/board.o board/logomatic/microsd.o
COREOBJS=core/tty.o core/parse.o core/system.o core/cli.o core/device.o core/timer.o core/clock.o core/file.o core/irq.o core/memory.o # core/filesystem.o
LPCOBJS=lpc/scb.o lpc/pll.o lpc/mam.o lpc/vpb.o lpc/vic.o lpc/pinsel.o lpc/timer.o lpc/gpio.o lpc/rtc.o lpc/ssp.o lpc/spi.o lpc/uart.o lpc/device.o lpc/eint.o lpc/pin.o lpc/pcon.o lpc/wdt.o lpc/reset.o lpc/stubs.o
SENSOROBJ=sensor/scp.o sensor/nmea.o
CMDOBJS=commands/gpio.o commands/mem.o commands/sys.o commands/nmea.o commands/posix.o commands/gps.o commands/lpc.o
FATOBJS=fat/fat16.o fat/partition.o fat/rootdir.o fat/sd_raw.o
POSIXOBJS=posix/process.o posix/epoll.o posix/file_console.o posix/signal.o posix/control.o
POSIXSYS=posix/sys_errno.o posix/sys_errlist.o posix/sys_file.o posix/sys_sleep.o posix/sys_mcontext.o posix/sys_ucontext.o posix/sys_sysconf.o posix/sys_mmap.o posix/sys_munmap.o posix/sys_mremap.o posix/sys_getpagesize.o
DISARMOBJS=pkg/libdisarm/src/libdisarm/args.o pkg/libdisarm/src/libdisarm/print.o pkg/libdisarm/src/libdisarm/parser.o
OBJS=$(COREOBJS) $(LPCOBJS) $(CMDOBJS) $(LOGOMATICOBJS) $(POSIXOBJS) $(SENSOROBJ) $(POSIXSYS) $(DISARMOBJS) $(FATOBJS) main.o serial_fifo.o vcom.o
INCLUDEFLAGS=-Iinclude -Ipkg/lpcusb/target -Ipkg/dietlibc/include -Ipkg/libdisarm/src -I.
DEFINE=-DLPC214x -DNDEBUG -DLPC_MEMMAP=MEM_MAP_USER_RAM

DEPS=$(OBJS:.o=.p)
LSTS=$(OBJS:.o=.lst)

TAGSRCS=$(shell find . -name '*.c' -or -name '*.h')

.DELETE_ON_ERROR:

all: depend tags bike.elf bike.hex bike.bin bike.size bike.lst

tags: TAGS

depend: $(DEPS)

TAGS: $(TAGSRCS)
	@echo "Regenerating tags file..."
	@etags $(TAGSRCS)

pkg/dietlibc/bin-arm/dietlibc.a pkg/dietlibc/bin-arm/libm.a:
	export CROSS=$(CROSS) && cd pkg/dietlibc && make clean && make all

pkg/lpcusb/target/usbstack.a:
	export CROSS=$(CROSS) && cd pkg/lpcusb/target && make clean && make depend && make lib

bike.elf bike.map: $(OBJS) $(LIBS) $(LDSCRIPT)
	$(CC) $(LDFLAGS) -T $(LDSCRIPT) \
		-Wl,-Map="bike.map" -o bike.elf \
		-Wl,--start-group -lgcc $(OBJS) $(LIBS) -Wl,--end-group

bike.size: bike.elf
	$(SIZE) bike.elf | tee bike.size

clean:
	rm -f $(DEPS) $(OBJS) $(LSTS) bike.elf bike.map bike.bin bike.hex bike.lst bike.size TAGS
.PHONY: clean

cleanlibs:
	rm -rf dietlibc/bin-arm
	rm -f lpcusb-trunk/target/usbstack.a lpsusb-trunk/target/*.o
.PHONY: cleanlibs

##### GENERIC RULES #####

%.p: %.c
	@$(CC) $(CFLAGS) -M -o $(*).d $(<)
	@sed 's|.*\.o:|$(@:.p=.o): |g' < $*.d > $@; rm -f $*.d; [ -s $@ ] || rm -f $@

%.p: %.S
	@$(CC) $(ASFLAGS) -M -o $(*).d $(<)
	@sed 's|.*\.o:|$(@:.p=.o): |g' < $*.d > $@; rm -f $*.d; [ -s $@ ] || rm -f $@

%.o: %.c
	$(CC) $(CFLAGS) -Wa,-adhlns=$(@:.o=.lst) -c -o $@ $<

%.o: %.S
	$(CC) $(ASFLAGS) -Wa,-adhlns=$(@:.o=.lst) -c -o $@ $<

%.bin: %.elf
	$(OBJCOPY) --gap-fill=0xff -O binary $^ $@

%.hex: %.elf
	$(OBJCOPY) --gap-fill=0xff -O ihex $^ $@

%.lst: %.elf
	$(OBJDUMP) -d $^ > $@

-include $(DEPS)

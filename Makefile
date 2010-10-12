
CROSS?=arm-elf-

CC=$(CROSS)gcc
SIZE=$(CROSS)size
OBJCOPY=$(CROSS)objcopy
OBJDUMP=$(CROSS)objdump

CFLAGS_CONFIG=-gdwarf-2 -Os -ffunction-sections
CFLAGS_TARGET=-mcpu=arm7tdmi -mfloat-abi=soft -mno-thumb-interwork
CFLAGS_WARNINGS=-Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align -Wimplicit -Wunused -Wredundant-decls -Wnested-externs -Wbad-function-cast -Wsign-compare -Waggregate-return

CFLAGS=$(CFLAGS_CONFIG) $(CFLAGS_TARGET) $(CFLAGS_WARNINGS) $(INCLUDEFLAGS) $(DEFINE)
ASFLAGS=-D__ASSEMBLY__ $(DEFINE) $(INCLUDEFLAGS)
LDFLAGS=-static -static-libgcc -nostartfiles -nostdlib -nodefaultlibs -Wl,--gc-sections -Wl,--cref

LDSCRIPT=bike.lds

LIBS=lpcusb-trunk/target/usbstack.a dietlibc/bin-arm/dietlibc.a

LOGOMATICOBJS=board/logomatic.o
COREOBJS=core/tick.o core/tty.o
LPCOBJS=lpc/scb.o lpc/pll.o lpc/mam.o lpc/vpb.o lpc/vic.o lpc/pinsel.o lpc/timer.o lpc/gpio.o lpc/rtc.o lpc/ssp.o lpc/spi.o lpc/uart.o lpc/device.o lpc/eint.o lpc/pin.o
OBJS=start.o $(COREOBJS) $(LPCOBJS) $(LOGOMATICOBJS) halsys.o main.o serial_fifo.o armVIC.o console.o printf.o nmea.o vcom.o scp.o
INCLUDEFLAGS=-Ilpcusb-trunk/target -Idietlibc/include -I.
DEFINE=-DLPC214x -DDEBUG

DEPS=$(OBJS:.o=.p)
LSTS=$(OBJS:.o=.lst)

.DELETE_ON_ERROR:

all: depend bike.elf bike.hex bike.bin bike.size bike.lst

depend: $(DEPS)


dietlibc/bin-arm/dietlibc.a dietlibc/bin-arm/libm.a:
	cd dietlibc && make clean && make all

lpcusb-trunk/target/usbstack.a:
	cd lpcusb-trunk/target && make clean && make depend && make lib

bike.elf bike.map: $(OBJS) $(LIBS) $(LDSCRIPT)
	$(CC) $(LDFLAGS) -T $(LDSCRIPT) \
		-Wl,-Map="bike.map" -o bike.elf \
		-Wl,--start-group -lgcc $(OBJS) $(LIBS) -Wl,--end-group

bike.size: bike.elf
	$(SIZE) bike.elf | tee bike.size

clean:
	rm -f $(DEPS) $(OBJS) $(LSTS) bike.elf bike.map bike.bin bike.hex bike.lst bike.size
.PHONY: clean

cleanlibs:
	rm -f dietlibc/bin-arm/dietlibc.a dietlibc/bin-arm/libm.a
	rm -f lpcusb-trunk/target/usbstack.a
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

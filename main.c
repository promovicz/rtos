/*
	LPCUSB, an USB device driver for LPC microcontrollers	
	Copyright (C) 2006 Bertrik Sikken (bertrik@sikken.nl)

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright
	   notice, this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright
	   notice, this list of conditions and the following disclaimer in the
	   documentation and/or other materials provided with the distribution.
	3. The name of the author may not be used to endorse or promote products
	   derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
	IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, 
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
	THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
	Minimal implementation of a USB serial port, using the CDC class.
	This example application simply echoes everything it receives right back
	to the host.

	Windows:
	Extract the usbser.sys file from .cab file in C:\WINDOWS\Driver Cache\i386
	and store it somewhere (C:\temp is a good place) along with the usbser.inf
	file. Then plug in the LPC214x and direct windows to the usbser driver.
	Windows then creates an extra COMx port that you can open in a terminal
	program, like hyperterminal.

	Linux:
	The device should be recognised automatically by the cdc_acm driver,
	which creates a /dev/ttyACMx device file that acts just like a regular
	serial port.

*/

int errno;

#include <string.h>
#include <stdio.h>

#include <core/defines.h>
#include <core/tty.h>
#include <core/tick.h>

#include <board/logomatic.h>

#include <lpc/vic.h>
#include <lpc/pll.h>
#include <lpc/ssp.h>
#include <lpc/eint.h>
#include <lpc/uart.h>
#include <lpc/gpio.h>
#include <lpc/timer.h>
#include <lpc/pinsel.h>
#include <lpc/spi.h>

#include <posix/control.h>

#include <commands/commands.h>

#include "type.h"
#include "debug.h"
#include "lpc214x.h"
#include "armVIC.h"
#include "hal.h"
#include "console.h"
#include "usbapi.h"
#include "vcom.h"
#include "serial_fifo.h"
#include "nmea.h"
#include "scp.h"

#define BAUD_RATE	115200

#define INTV_STOP   0
#define	INTV_USB	1
#define INTV_UART0  2
#define INTV_UART1  3
#define INTV_TIMER0 4
#define INTV_TIMER1 5

#define CSEL_SCP 20
#define CSEL_MMC 7


#define IRQ_MASK 0x00000080


static interrupt_handler void DefIntHandler(void)
{
	vic_ack();
}

static interrupt_handler void UART0IntHandler(void)
{
	uart_irq(UART0);
	vic_ack();
}

static interrupt_handler void UART1IntHandler(void)
{
	uart_irq(UART1);
	vic_ack();
}

static interrupt_handler void USBIntHandler(void)
{
	USBHwISR();
	vic_ack();
}

static interrupt_handler void Timer0IntHandler(void)
{
	timer_irq(0);
	vic_ack();
}

static interrupt_handler void Timer1IntHandler(void)
{
	timer_irq(1);
	vic_ack();
}

static interrupt_handler void e0IntHandler(void)
{
	eint_irq(0);
	vic_ack();
}

static interrupt_handler void e1IntHandler(void)
{
	eint_irq(1);
	vic_ack();
}

static interrupt_handler void e2IntHandler(void)
{
	eint_irq(2);
	vic_ack();
}

static interrupt_handler void e3IntHandler(void)
{
	eint_irq(3);
	vic_ack();
}


int icount[2];

int consirq = 0;

struct tty tser;

void t0match(int t, timer_match_t m)
{
	tick_handler();

	tick_t time = tick_get();
	if(!(time%TICK_SECOND)) {
		led_stat1((time/TICK_SECOND)&1);
	}
}

void stop_handler(int eint)
{
	led_stat2(1);
}

void command_handler(struct tty *t, int argc, char **argv)
{
	if(argc) {
		if(!strcmp("gps", argv[0])) {
			nmea_command(t, argc-1, argv+1);
		}
		if(!strcmp("spi", argv[0])) {
			spi_command(t, argc-1, argv+1);
		}
		if(!strcmp("ssp", argv[0])) {
			ssp_command(t, argc-1, argv+1);
		}
		if(!strcmp("mem", argv[0])) {
			mem_command(t, argc-1, argv+1);
		}
		if(!strcmp("gpio", argv[0])) {
			gpio_command(t, argc-1, argv+1);
		}
		if(!strcmp("power", argv[0])) {
			power_command(t, argc-1, argv+1);
		}
		if(!strcmp("vic", argv[0])) {
			if(argc > 1) {
				if(!strcmp("report", argv[1])) {
					vic_report();
				}
			}
		}
		if(!strcmp("scp", argv[0])) {
			if(argc > 1) {
				if(!strcmp("reset", argv[1])) {
					scp_init();
				}
				if(!strcmp("measure", argv[1])) {
					scp_measure();
				}
				if(!strcmp("selftest", argv[1])) {
					scp_selftest();
				}
			}
		}
		if(!strcmp("time", argv[0])) {
			tick_t t = tick_get();
			printf("sys time %07d.%03d\n", t/1000, t%1000);
		}
		if(!strcmp("pll", argv[0])) {
			pll_report();
		}
		if(!strcmp("pin", argv[0])) {
			pin_report();
		}

		if(!strcmp("cwrite", argv[0])) {
			int c = 10;
			while(c-- > 0) {
				const char *str = "fnord\r\n";
				int c = write(1, str, strlen(str));
				tick_delay(1*TICK_SECOND);
			}
		}

		if(!strcmp("cread", argv[0])) {
			int c = 10;
			while(c-- > 0) {
				const char buf[32];
				int c = read(0, buf, sizeof(buf));
				printf("got %d bytes\r\n", c);
				tick_delay(1*TICK_SECOND);
			}
		}
	}
}

void csel_scp(bool_t yeah)
{
	if(yeah) {
		gpio_pin_low(0, CSEL_SCP);
	} else {
		gpio_pin_high(0, CSEL_SCP);
	}
}

void csel_mmc(bool_t yeah)
{
	if(yeah) {
		gpio_pin_low(0, CSEL_MMC);
	} else {
		gpio_pin_high(0, CSEL_MMC);
	}
}

int main (void)
{
	board_init();

	console_init();

	uart_init(UART0, 115200);
	uart_init(UART1, 9600);

	ssp_init();
	ssp_clock(100000);
	ssp_enable(TRUE);

	vcom_init();

	vic_configure(INTV_STOP, INT_EINT1, &e1IntHandler);
	vic_configure(INTV_USB, INT_USB, &USBIntHandler);
	vic_configure(INTV_UART0, INT_UART0, &UART0IntHandler);
	vic_configure(INTV_UART1, INT_UART1, &UART1IntHandler);
	vic_configure(INTV_TIMER0, INT_TIMER0, &Timer0IntHandler);
	vic_configure(INTV_TIMER1, INT_TIMER1, &Timer1IntHandler);

	gpio_pin_high(0, CSEL_SCP);
	gpio_pin_set_direction(0, CSEL_SCP, BOOL_TRUE);
	gpio_pin_high(0, CSEL_MMC);
	gpio_pin_set_direction(0, CSEL_MMC, BOOL_TRUE);

	eint_handler(EINT1, &stop_handler);

	vic_enable(INT_EINT1);
	vic_enable(INT_USB);
	vic_enable(INT_UART0);
	vic_enable(INT_UART1);
	vic_enable(INT_TIMER0);

	timer_init(TIMER0);
	timer_prescale(TIMER0, 60);

	timer_match_configure(TIMER0, MR0, 1000, TIMER_MATCH_INTERRUPT|TIMER_MATCH_RESET);
	timer_match_handler(TIMER0, MR0, &t0match);

	timer_enable(TIMER0, TRUE);

	enableIRQ();

	vcom_connect(BOOL_TRUE);

	consirq = 1;

	nmea_init();

	tty_init(&tser);
	tty_command_handler(&tser, &command_handler);

	scp_init();

	uint8_t chr;
	while (1) {
		if(read(0, &chr, 1)) {
			tty_feed(&tser, chr);
		}

		fflush(stdout);

		if(uart_rx_fifo(0, &chr, 1)) {
		}

		if(uart_rx_fifo(1, &chr, 1)) {
			nmea_process(&chr, 1);
		}
	}

	return 0;
}

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

#include <string.h>
#include <stdio.h>
#include <sys/ucontext.h>

#include <core/defines.h>
#include <core/tty.h>
#include <core/cli.h>
#include <core/timer.h>
#include <core/system.h>

#include <board/logomatic/board.h>

#include <lpc/vic.h>
#include <lpc/pll.h>
#include <lpc/ssp.h>
#include <lpc/eint.h>
#include <lpc/uart.h>
#include <lpc/gpio.h>
#include <lpc/timer.h>
#include <lpc/pinsel.h>
#include <lpc/spi.h>
#include <lpc/wdt.h>

#include <posix/control.h>

#include "type.h"
#include "debug.h"
#include "lpc214x.h"
#include "vcom.h"
#include "serial_fifo.h"

#include <sensor/scp.h>
#include <sensor/nmea.h>

#include <sys/mman.h>

#include <usbapi.h>

#define INTV_STOP     0
#define	INTV_USB	  1
#define INTV_UART0    2
#define INTV_UART1    3
#define INTV_TIMER0   4
#define INTV_TIMER1   5
#define INTV_SCP_DRDY 6

#define CE_NRF 25

#define CSEL_NRF 28
#define CSEL_SCP 20
#define CSEL_MMC 29

void csel_scp(bool_t yeah)
{
	gpio_pin_set(CSEL_SCP, yeah ? 0 : 1);
}

void csel_mmc(bool_t yeah)
{
	gpio_pin_set(CSEL_MMC, yeah ? 0 : 1);
}

static interrupt_handler void DefIntHandler(void)
{
	vic_ack();
}

static interrupt_handler void USBIntHandler(void)
{
	USBHwISR();
	vic_ack();
}

int icount[2];

int consirq = 0;

struct tty tser;

struct cli c;

void command_handler(struct tty *t, int argc, char **argv)
{
	led_stat2(0);

	cli_execute(&c, argc, argv);
}

void help_handler(struct tty *t, int argc, char **argv)
{
	led_stat2(0);

	cli_help(&c, argc, argv);
}

int u0 = -1, u1 = -1;

int hit = 0;
int bang = 0;

ucontext_t ou;


ucontext_t nmea_context;
uint8_t nmea_stack[1024];

void nmea_main (void)
{
	char buf[64];
	int r;

	nmea_init();

	while(1) {
		r = read(u0, &buf, sizeof(buf));
		if(r > 0) {
			nmea_process(&buf[0], r);
		}
		swapcontext(&nmea_context, &ou);
	}
}

ucontext_t rimu_context;
uint8_t rimu_stack[1024];

void rimu_main (void)
{
	uint8_t buf[64];
	int r;

	{
		uint8_t cmd_raw = '4';
		write(u1, &cmd_raw, 1);
	}

	while(1) {
		r = read(u1, &buf, sizeof(buf));
		if(r > 0) {
		}
		swapcontext(&rimu_context, &ou);
	}
}

void rimu_proc_init (void)
{
	rimu_context.uc_stack.ss_sp = &rimu_stack;
	rimu_context.uc_stack.ss_size = sizeof(rimu_stack);
	rimu_context.uc_link = NULL;
	makecontext(&rimu_context, &rimu_main, 0);
}

void nmea_proc_init (void)
{
	nmea_context.uc_stack.ss_sp = &nmea_stack;
	nmea_context.uc_stack.ss_size = sizeof(nmea_stack);
	nmea_context.uc_link = NULL;
	makecontext(&nmea_context, &nmea_main, 0);
}


int main (void)
{
	c.commands = &cli_system_commands;

	gpio_pin_set(CSEL_SCP, 1);
	gpio_pin_set_direction(CSEL_SCP, BOOL_TRUE);

	system_init();

	vcom_init();

	vic_configure(INTV_USB, INT_USB, &USBIntHandler);
	vic_enable(INT_USB);

	vcom_connect(BOOL_TRUE);

	posix_console_enable();

	tty_init(&tser);
	tty_command_handler(&tser, &command_handler, &help_handler);

	u0 = device_open("uart0", O_RDWR);
	u1 = device_open("uart1", O_RDWR);

	nmea_proc_init();
	rimu_proc_init();

	int i;
	uint8_t chr;
	char buf[64];
	int res;
	while (1) {
		system_kick();

		if(read(0, &chr, 1)) {
			tty_feed(&tser, chr);
		}

		fflush(stdout);

		swapcontext(&ou, &nmea_context);

		fflush(stdout);

		swapcontext(&ou, &rimu_context);

		fflush(stdout);

		system_idle();
	}

	return 0;
}

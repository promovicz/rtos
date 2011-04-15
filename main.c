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

#include <core/defines.h>
#include <core/tty.h>
#include <core/cli.h>
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
#include <lpc/wdt.h>

#include <posix/control.h>

#include <commands/commands.h>

#include "type.h"
#include "debug.h"
#include "lpc214x.h"
#include "armVIC.h"
#include "hal.h"
#include "vcom.h"
#include "serial_fifo.h"

#include <sensor/scp.h>

#include <sys/mman.h>

#define INTV_STOP     0
#define	INTV_USB	  1
#define INTV_UART0    2
#define INTV_UART1    3
#define INTV_TIMER0   4
#define INTV_TIMER1   5
#define INTV_SCP_DRDY 6

#define CE_NRF 25

#define CSEL_NRF 28
#define CSEL_SCP 29
#define CSEL_MMC 7


static interrupt_handler void DefIntHandler(void)
{
	vic_ack();
}

static interrupt_handler void UART0IntHandler(void)
{
	uart_irq(0);
	vic_ack();
}

static interrupt_handler void UART1IntHandler(void)
{
	uart_irq(1);
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

void scp_drdy_handler(int eint)
{
	scp_irq();
}

int command_scpm(struct cli *c, int argc, char **argv)
{
	scp_measure();
	return 0;
}

struct command cmd_root[] = {
	{
		.name = "system",
		.help = "system",
		.handler = NULL,
		.children = &cmds_system
	},
	{
		.name = "posix", 
		.help = "posix emulator",
		.handler = NULL,
		.children = &cmds_posix
	},
	{
		.name = "gpio",
		.help = "gpio pins",
		.handler = NULL,
		.children = &cmds_gpio
	},
#if 0
	{
		.name = "gps",
		.help = "gps receiver",
		.handler = NULL,
		.children = &cmds_gps
	},
	{
		.name = "nmea",
		.help = "nmea receiver",
		.handler = NULL,
		.children = &cmds_nmea
	},
#endif
	{
		.name = "mem",
		.help = "memory operations",
		.handler = NULL,
		.children = &cmds_mem
	},
	{
		.name = "lpc",
		.help = "lpc platform",
		.handler = NULL,
		.children = &cmds_lpc
	},
#if 0
	{
		.name = "scpm",
		.help = "scp measure",
		.handler = &command_scpm,
	},
#endif
};

DECLARE_COMMAND_TABLE(cmds_root, cmd_root);

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

void csel_scp(bool_t yeah)
{
	if(yeah) {
		gpio_pin_low(CSEL_SCP);
	} else {
		gpio_pin_high(CSEL_SCP);
	}
}

void csel_nrf(bool_t yeah)
{
	if(yeah) {
		gpio_pin_low(CSEL_NRF);
	} else {
		gpio_pin_high(CSEL_NRF);
	}
}

void ce_nrf(bool_t yeah)
{
	if(yeah) {
		gpio_pin_low(CE_NRF);
	} else {
		gpio_pin_high(CE_NRF);
	}
}

void csel_mmc(bool_t yeah)
{
	if(yeah) {
		gpio_pin_low(CSEL_MMC);
	} else {
		gpio_pin_high(CSEL_MMC);
	}
}

int u0 = -1, u1 = -1;

void nmea_hook(const char *raw, size_t nbytes)
{
	if(u0 != -1) {
		write(u0, raw, nbytes);
	}
}

int main (void)
{
	c.commands = &cmds_root;

	system_init();

	uart_init(0, 38400);
	uart_init(1, 9600);

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
	vic_configure(INTV_SCP_DRDY, INT_EINT3, &e3IntHandler);

	gpio_pin_high(CSEL_SCP);
	gpio_pin_set_direction(CSEL_SCP, BOOL_TRUE);
	gpio_pin_high(CSEL_MMC);
	gpio_pin_set_direction(CSEL_MMC, BOOL_TRUE);

	gpio_pin_high(CSEL_NRF);
	gpio_pin_set_direction(CSEL_NRF, BOOL_TRUE);
	gpio_pin_high(CE_NRF);
	gpio_pin_set_direction(CE_NRF, BOOL_TRUE);


	eint_handler(EINT1, &stop_handler);

	pin_set_function(PIN30, PIN_FUNCTION_EINT3);
	eint_configure(EINT3, 1, 1);
	eint_handler(EINT3, &scp_drdy_handler);

	vic_enable(INT_EINT1);
	vic_enable(INT_USB);
	vic_enable(INT_UART0);
	vic_enable(INT_UART1);
	vic_enable(INT_TIMER0);
	vic_enable(INT_EINT3);

	timer_init(TIMER0);
	timer_prescale(TIMER0, 60);

	timer_match_configure(TIMER0, MR0, 1000, TIMER_MATCH_INTERRUPT|TIMER_MATCH_RESET);
	timer_match_handler(TIMER0, MR0, &t0match);

	timer_enable(TIMER0, TRUE);

	enableIRQ();

	vcom_connect(BOOL_TRUE);

	posix_console_enable();

	nmea_init();
	nmea_sentence_hook(&nmea_hook);

	tty_init(&tser);
	tty_command_handler(&tser, &command_handler, &help_handler);

	scp_init();

	u0 = uart_open(0);
	u1 = uart_open(1);

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

		res = read(u0, buf, sizeof(buf));
		if(res > 0) {
			write(u0, buf, res);
		}

		res = read(u1, buf, sizeof(buf));
		if(res > 0) {
			nmea_process(buf, res);
		}

		usleep(10000);
	}

	return 0;
}

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

#include "type.h"
#include "debug.h"

#include "lpc214x.h"

#include "armVIC.h"

#include "hal.h"
#include "console.h"
#include "usbapi.h"

#include "vic.h"

#include "uart.h"
#include "vcom.h"

#include "serial_fifo.h"

#include "nmea.h"

#include "gpio.h"

#include "timer.h"

#include "ssp.h"

#include "scp.h"

#include "cli.h"

#define BAUD_RATE	115200

#define	INTV_USB	0
#define INTV_UART0  1
#define INTV_UART1  2
#define INTV_TIMER0 3
#define INTV_TIMER1 4

#define LED_STAT0 2
#define LED_STAT1 11

#define CSEL_SCP1000 20


#define IRQ_MASK 0x00000080


// forward declaration of interrupt handler
static void USBIntHandler(void) __attribute__ ((interrupt("IRQ")));
static void UART0IntHandler(void) __attribute__ ((interrupt("IRQ")));
static void UART1IntHandler(void) __attribute__ ((interrupt("IRQ")));
static void Timer0IntHandler(void) __attribute__ ((interrupt("IRQ")));
static void Timer1IntHandler(void) __attribute__ ((interrupt("IRQ")));
static void DefIntHandler(void) __attribute__ ((interrupt("IRQ")));

static void DefIntHandler(void)
{
	VICVectAddr = 0x00;
}

static void UART0IntHandler(void)
{
	uart_irq(UART0);
	VICVectAddr = 0x00;
}

static void UART1IntHandler(void)
{
	uart_irq(UART1);
	VICVectAddr = 0x00;
}

static void USBIntHandler(void)
{
	USBHwISR();
	VICVectAddr = 0x00;
}

static void Timer0IntHandler(void)
{
	timer_irq(0);
	VICVectAddr = 0x00;
}

static void Timer1IntHandler(void)
{
	timer_irq(1);
	VICVectAddr = 0x00;
}


int icount[2];

int consirq = 0;

struct tty tser;

volatile uint32_t systime = 0;

void t0match(timer_t t, timer_match_t m)
{
	systime++;
	if(!(systime%1000)) {
		if((systime/1000)&1) {
			gpio_set(0, LED_STAT1);
		} else {
			gpio_clear(0, LED_STAT1);
		}
	}
}

void mdelay(uint32_t msec)
{
	uint32_t start = systime;
	uint32_t end = start + msec;
	while(systime <= end) { }
	return;
}

void command_handler(struct tty *t, int argc, char **argv)
{
	if(argc) {
		if(!strcmp("gps", argv[0])) {
			nmea_command(t, argc-1, argv+1);
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
			uint32_t t = systime;
			printf("sys time %07d.%03d\n", t/1000, t%1000);
		}
	}
}

void csel_scp(bool_t yeah)
{
	if(yeah) {
		gpio_clear(0, CSEL_SCP1000);
	} else {
		gpio_set(0, CSEL_SCP1000);
	}
}

int main (void)
{
	HalSysInit();

	PINSEL0 = (PINSEL0 & ~0x000FFF0F) | 0x00055505; // UART0, UART1, SPI
	PINSEL1 = (PINSEL1 & ~0x000003FC) | 0x000000A8; // SSP with SSEL as GPIO

	uart_init(UART0, 115200);
	uart_init(UART1, 9600);

	ssp_init();
	ssp_clock(100000);
	ssp_enable(TRUE);

	vcom_init();

	vic_configure(INTV_USB, INT_USB, &USBIntHandler);
	vic_configure(INTV_UART0, INT_UART0, &UART0IntHandler);
	vic_configure(INTV_UART1, INT_UART1, &UART1IntHandler);
	vic_configure(INTV_TIMER0, INT_TIMER0, &Timer0IntHandler);
	vic_configure(INTV_TIMER1, INT_TIMER1, &Timer1IntHandler);

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

	gpio_direction(0, LED_STAT0, TRUE);
	gpio_direction(0, LED_STAT1, TRUE);
	gpio_set(0, LED_STAT0);
	gpio_clear(0, LED_STAT1);

	gpio_direction(0, CSEL_SCP1000, TRUE);
	gpio_set(0, CSEL_SCP1000);

	scp_init();

	uint8_t chr;
	while (1) {
		if(vcom_rx_fifo(&chr)) {
			tty_feed(&tser, chr);
		}

		if(uart_rx_fifo(0, &chr)) {
		}

		if(uart_rx_fifo(1, &chr)) {
			nmea_process(&chr, 1);
		}
	}

	return 0;
}

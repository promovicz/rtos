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

#include "cli.h"

#define BAUD_RATE	115200

#define	INTV_USB	0
#define INTV_UART0  1
#define INTV_UART1  2

#define IRQ_MASK 0x00000080


// forward declaration of interrupt handler
static void USBIntHandler(void) __attribute__ ((interrupt("IRQ")));
static void UART0IntHandler(void) __attribute__ ((interrupt("IRQ")));
static void UART1IntHandler(void) __attribute__ ((interrupt("IRQ")));
static void DefIntHandler(void) __attribute__ ((interrupt("IRQ")));


int default_irq_count = 0;

static void DefIntHandler(void)
{
	default_irq_count++;
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


int icount[2];

int consirq = 0;

struct tty tser;


int main (void)
{
	HalSysInit();

	PINSEL0 = (PINSEL0 & ~0x000F000F) | 0x00050005;

	uart_init(UART0, 115200);
	uart_init(UART1, 9600);

	vcom_init();

	vic_configure(INTV_USB, INT_USB, &USBIntHandler);
	vic_configure(INTV_UART0, INT_UART0, &UART0IntHandler);
	vic_configure(INTV_UART1, INT_UART1, &UART1IntHandler);

	vic_enable(INT_USB);
	vic_enable(INT_UART0);
	vic_enable(INT_UART1);

	enableIRQ();

	vcom_connect(BOOL_TRUE);

	consirq = 1;

	nmea_init();

	tty_init(&tser);

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

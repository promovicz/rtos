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
#include <core/tick.h>
#include <core/timer.h>

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
	uint8_t stk[512];



void foo(void)
{
	int u = 0;

	hit = 1;
	printf("Otherland!\n");
	printf("u is at %p\n", &u);

	mcontext_t *mcp = &ou.uc_mcontext;
	stack_t *sp = &ou.uc_stack;
	mstack_t *state;

	state = mcp->mc_sp;

	printf("rst is %8.8p\n", state);
	printf("rpc is %8.8p\n", state->ms_pc);

	fflush(stdout);

	setcontext(&ou);
}

void bar(void)
{
	int u = 0;

	bang = 1;
	printf("Otherland2!\n");
	printf("u is at %p\n", &u);

	mcontext_t *mcp = &ou.uc_mcontext;
	stack_t *sp = &ou.uc_stack;
	mstack_t *state;

	state = mcp->mc_sp;

	printf("rst is %8.8p\n", state);
	printf("rpc is %8.8p\n", state->ms_pc);

	fflush(stdout);

	setcontext(&ou);
}


int main (void)
{
	c.commands = &cli_system_commands;

	system_init();

	vcom_init();

	vic_configure(INTV_USB, INT_USB, &USBIntHandler);
	vic_enable(INT_USB);

	vcom_connect(BOOL_TRUE);

	posix_console_enable();

	tty_init(&tser);
	tty_command_handler(&tser, &command_handler, &help_handler);

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

			printf("chk %d!\n", res);

			if(!hit) {
				printf("stk is at %p\n", &stk);
				fflush(stdout);

				ucontext_t u;
				u.uc_stack.ss_sp = &stk;
				u.uc_stack.ss_size = sizeof(stk);
				u.uc_link = NULL;
				makecontext(&u, &foo, 0);

				printf("swap1!\n");
				swapcontext(&ou, &u);

				printf("back1!\n");
			}
			else if(!bang) {
				printf("stk is at %p\n", &stk);
				fflush(stdout);

				ucontext_t u;
				u.uc_stack.ss_sp = &stk;
				u.uc_stack.ss_size = sizeof(stk);
				u.uc_link = NULL;
				makecontext(&u, &bar, 0);

				printf("swap2!\n");
				swapcontext(&ou, &u);

				printf("back2!\n");
			}

			printf("done!\n");

		}

		res = read(u1, buf, sizeof(buf));
		if(res > 0) {
			write(u1, buf, res);
		}

		//system_idle();
		usleep(10000);
	}

	return 0;
}

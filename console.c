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
	Simple console input/output, over serial port #0

	Partially copied from Jim Lynch's tutorial
*/

#include "console.h"

#include <lpc/uart.h>

#include "vcom.h"

#include <core/tty.h>

#include <posix/control.h>

#include <stdio.h>

extern int consirq; // XXX

static int vcom_write(struct file *f, const void *buf, size_t nbytes)
{
	if(consirq) {
		return vcom_tx_fifo(buf, nbytes);
	} else {
		return nbytes;
	}
}

static int vcom_read(struct file *f, void *buf, size_t nbytes)
{
	if(consirq) {
		return vcom_rx_fifo(buf, nbytes);
	} else {
		return nbytes;
	}
}

struct file_operations vcom_operations = {
	.fop_write = &vcom_write,
	.fop_read = &vcom_read,
};

void console_init(void)
{
	struct file *fi = file_alloc();
	struct file *fo = file_alloc();
	struct file *fe = file_alloc();

	/* initialize tables - must be done here to guarantee correct FD allocation */
	file_table_init();

	/* stdin */
	fi->f_used = 1;
	fi->f_flags = O_RDONLY;
	fi->f_ops = &vcom_operations;
	fd_alloc(fi); /* XXX ASSERT returns 0 */

	/* stdout */
	fo->f_used = 1;
	fo->f_flags = O_WRONLY;
	fo->f_ops = &vcom_operations;
	fd_alloc(fo); /* XXX ASSERT returns 1 */

	/* stderr */
	fe->f_used = 1;
	fe->f_flags = O_WRONLY;
	fe->f_ops = &vcom_operations;
	fd_alloc(fe); /* XXX ASSERT returns 2 */
}

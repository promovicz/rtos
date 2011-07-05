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

#include "serial_fifo.h"

#include <stdlib.h>

#include <core/memory.h>

static void *fifo_buf_alloc(const char *name, size_t size)
{
	if(size % PAGESIZE) {
		return malloc(size);
	} else {
		return memory_alloc_pages(size / PAGESIZE, name);
	}
}

void fifo_init(fifo_t *fifo, const char *name, size_t size)
{
	fifo->name = name ? (char*)name : "fifo";
	fifo->size = size;
	fifo->head = 0;
	fifo->tail = 0;
	fifo->buf = fifo_buf_alloc(fifo->name, size);
}


bool_t fifo_put(fifo_t *fifo, unsigned char c)
{
	int next;
	
	// check if FIFO has room
	next = (fifo->head + 1) % fifo->size;
	if (next == fifo->tail) {
		// full
		return BOOL_FALSE;
	}
	
	fifo->buf[fifo->head] = c;
	fifo->head = next;
	
	return BOOL_TRUE;
}


bool_t fifo_get(fifo_t *fifo, unsigned char *pc)
{
	int next;
	
	// check if FIFO has data
	if (fifo->head == fifo->tail) {
		return BOOL_FALSE;
	}
	
	next = (fifo->tail + 1) % fifo->size;
	
	*pc = fifo->buf[fifo->tail];
	fifo->tail = next;

	return BOOL_TRUE;
}


int fifo_avail(fifo_t *fifo)
{
	return (fifo->size + fifo->head - fifo->tail) % fifo->size;
}


int fifo_free(fifo_t *fifo)
{
	return (fifo->size - 1 - fifo_avail(fifo));
}



#include "uart.h"

#include "memory.h"

#include "serial_fifo.h"

#include <core/device.h>
#include <core/irq.h>
#include <core/memory.h>

#include <stdio.h>

static inline always_inline uint8_t readb(volatile uint8_t *a)
{
	return *a;
}

static inline always_inline void writeb(uint8_t v, volatile uint8_t *a)
{
	*a = v;
}

/* register flags used for latched register addressing */
#define REG_FLAGS  0xFF80
#define REG_DLAB0  0x0080
#define REG_DLAB1  0x0100

/* base addresses of uarts */
#define UART0_BASE   ((void*)0xe000c000)
#define UART1_BASE   ((void*)0xe0010000)

/* register address computation */
#define UART_REG(u,r) ((volatile uint8_t*)(((uint8_t*)uarts[u].base) + ((r) & ~(REG_FLAGS))))

/* state structures */
struct uart {
	struct device dev;
	int index;
	void *base;

	fifo_t rxfifo;
	int rxmaxfill;
	int rxcount;

	fifo_t txfifo;
	int txmaxfill;
	int txcount;
};

static void uart_device_report(struct device *dev)
{
	struct uart *u = container_of(dev, struct uart, dev);
	printf(" rx count %d maxfill %d\n",
		   u->rxcount, u->rxmaxfill);
	printf(" tx count %d maxfill %d\n",
		   u->txcount, u->txmaxfill);
}

static int uart_open(struct file *file)
{
	struct device *dev = file->f_device;
	struct uart *u = container_of(dev, struct uart, dev);

	file->f_cookie_int = u->index;

	dev->uses++;

	return 0;
}

static int uart_close(struct file *file)
{
	struct device *dev = file->f_device;

	dev->uses--;

	return 0;
}

static ssize_t uart_write(struct file *f, const void *buf, size_t nbytes)
{
	return uart_tx_fifo(f->f_cookie_int, buf, nbytes);
}

static ssize_t uart_read(struct file *f, void *buf, size_t nbytes)
{
	return uart_rx_fifo(f->f_cookie_int, buf, nbytes);
}

struct file_operations uart_operations = {
	.fop_open = &uart_open,
	.fop_write = &uart_write,
	.fop_read = &uart_read,
	.fop_close = &uart_close,
};

struct uart uarts[2] = {
	{ .dev = { .name = "uart0",
			   .class = DEVICE_CLASS_STREAM,
			   .fops = &uart_operations,
			   .report_cb = &uart_device_report },
	  .index = 0,
	  .base = UART0_BASE,
	},
	{ .dev = { .name = "uart1",
			   .class = DEVICE_CLASS_STREAM,
			   .fops = &uart_operations,
			   .report_cb = &uart_device_report },
	  .index = 1,
	  .base = UART1_BASE,
	},
};

struct uart_regs {
	/* 0x00 */
	union {
		uint8_t RBR; /* ro */
		uint8_t THR; /* wo */
		uint8_t DLL; /* rw, dlab */
	};
	uint8_t _pad0[3];
	/* 0x04 */
	union {
		uint8_t IER; /* rw */
		uint8_t DLM; /* rw, dlab */
	};
	uint8_t _pad1[3];
	/* 0x08 */
	union {
		uint8_t IIR; /* ro */
		uint8_t FCR; /* wo */
	};
	uint8_t _pad2[3];
	/* 0x0c */
	uint8_t LCR; /* rw */
	uint8_t _pad3[3];
	/* 0x10 */
	uint8_t MCR; /* rw, modem control */
	uint8_t _pad4[3];
	/* 0x14 */
	uint8_t LSR; /* ro */
	uint8_t _pad5[3];
	/* 0x18 */
	uint8_t MSR; /* ro, modem control */
	uint8_t _pad6[3];
	/* 0x1c */
	uint8_t SCR; /* rw */
	uint8_t _pad7[3];
	/* 0x20 */
	uint32_t ACR; /* rw */
	uint8_t _pad8[4];
	/* 0x28 */
	uint32_t FDR; /* rw */
	uint8_t _pad9[4];
	/* 0x30 */
	uint8_t TER; /* rw */
	uint8_t _pad10[3];
};

/* register definitions */
enum uart_reg {
	RBR = 0x00 | REG_DLAB0, /* r */
	THR = 0x00 | REG_DLAB0, /* w */
	DLL = 0x00 | REG_DLAB1, /* rw */
	IER = 0x04 | REG_DLAB0, /* rw */
	DLM = 0x04 | REG_DLAB1, /* rw */
	IIR = 0x08, /* r */
	FCR = 0x08, /* w */
	LCR = 0x0c, /* rw */
	MCR = 0x10, /* rw, LPC2148: only second uart */
	LSR = 0x14, /* r */
	MSR = 0x18, /* r, LPC2148: only second uart */
	SCR = 0x1c, /* rw */
	ACR = 0x20, /* rw, 32bit */
	FDR = 0x28, /* rw, 32bit */
	TER = 0x30  /* rw */
};

/* register bit definitions (as needed) */
enum uart_ier_bits {
	IER_RBR = (1<<0),
	IER_THRE = (1<<1),
	IER_RLS = (1<<2),
	IER_MODEM = (1<<3),
	IER_CTS = (1<<7),
};
enum uart_iir_bits {
	IIR_IP = (1<<0),

	IIR_II_MODEM = 0x00,
	IIR_II_THRE = 0x02,
	IIR_II_RDA = 0x04,
	IIR_II_RLS = 0x06,
	IIR_II_CTI = 0x0C,
	IIR_II = 0x0E,

	IIR_FE = 0xC0,
};
enum uart_fcr_bits {
	FCR_ENABLE   = (1<<0),

	FCR_RX_RESET = (1<<1),
	FCR_TX_RESET = (1<<2),

	FCR_RX_TRIG_1  = 0x00,
	FCR_RX_TRIG_4  = 0x40,
	FCR_RX_TRIG_8  = 0x80,
	FCR_RX_TRIG_14 = 0xC0,
	FCR_RX_TRIG    = 0xC0,
};
enum uart_lcr_bits {
	LCR_WL_5    = 0x00,
	LCR_WL_6    = 0x01,
	LCR_WL_7    = 0x02,
	LCR_WL_8    = 0x03,
	LCR_WL      = 0x03,

	LCR_SB_1    = 0x00,
	LCR_SB_2    = 0x04,
	LCR_SB      = 0x04,

	LCR_PARE    = 0x08,

	LCR_PARS_ODD  = 0x00,
	LCR_PARS_EVEN = 0x10,
	LCR_PARS_1    = 0x20,
	LCR_PARS_0    = 0x30,
	LCR_PARS      = 0x30,

	LCR_PAR_NONE = 0x00,
	LCR_PAR_ODD  = LCR_PARE|LCR_PARS_ODD,
	LCR_PAR_EVEN = LCR_PARE|LCR_PARS_EVEN,

	LCR_BREAK     = 0x40,

	LCR_DLAB      = 0x80,
};
enum uart_lsr_bits {
	LSR_RDR  = (1<<0),
	LSR_OE   = (1<<1),
	LSR_PE   = (1<<2),
	LSR_FE   = (1<<3),
	LSR_BI   = (1<<4),
	LSR_THRE = (1<<5),
	LSR_TEMT = (1<<6),
	LSR_RXFE = (1<<7),
};

/* set the DLAB bit to the given state */
static inline void uart_set_dlab(uart_t uart, bool_t on)
{
	uint8_t r;

	r = readb(UART_REG(uart, LCR));
	if(on) {
		r |=  LCR_DLAB;
	} else {
		r &= ~LCR_DLAB;
	}
	writeb(r, UART_REG(uart,LCR));
}

/* write given value to given uart register */
static inline void uart_reg_write(uart_t uart, enum uart_reg r, uint8_t val)
{
	writeb(val, UART_REG(uart, r));
}

/* read from given uart register */
static inline uint8_t uart_reg_read(uart_t uart, enum uart_reg r)
{
	return readb(UART_REG(uart, r));
}

void uart_init(uart_t uart, uart_baud_t baudrate)
{
	struct uart *u = &uarts[uart];
	uint16_t div = (60000000 / (16 * baudrate));

	uart_reg_write(uart, LCR, LCR_WL_8|LCR_SB_1|LCR_PAR_NONE);

	uart_set_dlab(uart, 1);

	uart_reg_write(uart, DLL, div & 0xFF);
	uart_reg_write(uart, DLM, (div>>8) & 0xFF);

	uart_set_dlab(uart, 0);

	uart_reg_write(uart, FCR, FCR_ENABLE|FCR_RX_TRIG_1);

	fifo_init(&uarts[uart].rxfifo, "uart.rx", 128);
	fifo_init(&uarts[uart].txfifo, "uart.tx", 128);

	uart_reg_write(uart, IER, IER_RBR);

	device_add(&uarts[uart].dev);

	u->txmaxfill = 0;
	u->txcount = 0;
	u->rxmaxfill = 0;
	u->rxcount = 0;
}

void uart_irq(uart_t uart)
{
	uint8_t ind = readb(UART_REG(uart, IIR));
	uint8_t w;

	if(ind&IIR_II_CTI) {
	}

	if(ind&IIR_II_RDA) {
		while(uart_rx_nonblocking(uart, &w, 1)) {
			if(!fifo_put(&uarts[uart].rxfifo, w)) {
				break;
			}
		}
	}

	if(ind&IIR_II_THRE) {
		while (uart_reg_read(uart, LSR) & LSR_THRE) {
			if(!fifo_get(&uarts[uart].txfifo, &w)) {
				uint8_t m = uart_reg_read(uart, IER);
				m &= ~IER_THRE;
				uart_reg_write(uart, IER, m);
				break;
			}
			uart_reg_write(uart, THR, w);
		}
	}

	if(ind&IIR_II_RLS) {

	}

	if(ind&IIR_II_MODEM) {

	}
}

int uart_tx_blocking(uart_t uart, const void *buf, size_t nbytes)
{
	size_t done;
	uint8_t c;

	for(done = 0; done < nbytes; done++) {
		c = ((uint8_t *)buf)[done];
		while (!(readb(UART_REG(uart, LSR)) & LSR_THRE)) { }
		writeb(c, UART_REG(uart, THR));
	}

	return done;
}

int uart_tx_nonblocking(uart_t uart, const void *buf, size_t nbytes)
{
	size_t done;
	bool_t res;
	uint8_t c;

	for(done = 0; done < nbytes; done++) {
		c = ((uint8_t *)buf)[done];
		res = readb(UART_REG(uart, LSR)) & LSR_THRE;
		if(res) {
			writeb(c, UART_REG(uart, THR));
		} else {
			break;
		}
	}
	return done;
}

int uart_tx_fifo(uart_t uart, const void *buf, size_t nbytes)
{
	struct uart *u = &uarts[uart];
	bool_t starting, success;
	size_t done, donedirect;
	int avail;
	uint32_t m;
	uint8_t c;
	uint8_t ier;

	done = donedirect = 0;

	m = irq_disable();

	starting = readb(UART_REG(uart, LSR)) & LSR_TEMT;

	if(starting) {
		donedirect = uart_tx_nonblocking(uart, buf, nbytes);
	}

	done = donedirect;

	for( ; done < nbytes; done++) {
		c = ((uint8_t *)buf)[done];
		success = fifo_put(&uarts[uart].txfifo, c);

		if(!success) {
			break;
		}
	}

	if(done > 0) {
		ier = uart_reg_read(uart, IER);
		ier |= IER_THRE;
		uart_reg_write(uart, IER, ier);
	}

	avail = fifo_avail(&u->txfifo);
	if(avail > u->txmaxfill) {
		u->txmaxfill = avail;
	}

	u->txcount += done;

	irq_restore(m);

	return done;
}

int uart_rx_blocking(uart_t uart, void *buf, size_t nbytes)
{
	uint8_t *cbuf = (uint8_t *)buf;
	size_t done;

	for(done = 0; done < nbytes; done++) {
		while(!(readb(UART_REG(uart, LSR)) & LSR_RDR)) { }
		cbuf[done] = readb(UART_REG(uart, RBR));
	}

	return done;
}

int uart_rx_nonblocking(uart_t uart, void *buf, size_t nbytes)
{
	uint8_t *cbuf = (uint8_t *)buf;
	size_t done;

	for(done = 0; done < nbytes; done++) {
		bool_t res = readb(UART_REG(uart, LSR)) & LSR_RDR;
		if(res) {
			cbuf[done] = readb(UART_REG(uart, RBR));
		} else {
			break;
		}
	}
	return done;
}

int uart_rx_fifo(uart_t uart, void *buf, size_t nbytes)
{
	struct uart *u = &uarts[uart];
	uint8_t *cbuf = (uint8_t *)buf;
	int avail;
	size_t done;
	uint32_t m;

	m = irq_disable();

	avail = fifo_avail(&u->rxfifo);
	if(avail > u->rxmaxfill) {
		u->rxmaxfill = avail;
	}

	for(done = 0; done < nbytes; done++) {
		bool_t r = fifo_get(&uarts[uart].rxfifo, &cbuf[done]);
		if(!r) {
			break;
		}
	}

	u->rxcount += done;

	irq_restore(m);

	return done;
}

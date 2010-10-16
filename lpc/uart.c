
#include "uart.h"

#include "memory.h"

#include "armVIC.h"

#include "serial_fifo.h"

/* XXX find some better method for dealing with DLAB bullpoo */

#include <core/defines.h>

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
#define UART0_BASE   0xe000c000
#define UART1_BASE   0xe0010000

/* register address computation */
#define UART_REG(u,r) ((uint8_t*)(((uint8_t*)uarts[u].base) + ((r) & ~(REG_FLAGS))))

/* state structures */
char u0_rxbuf[VCOM_FIFO_SIZE];
char u0_txbuf[VCOM_FIFO_SIZE];
char u1_rxbuf[VCOM_FIFO_SIZE];
char u1_txbuf[VCOM_FIFO_SIZE];

struct uart {
	void *base;
	fifo_t rxfifo;
	char *rxbuf;
	fifo_t txfifo;
	char *txbuf;
};

struct uart uarts[2] = {
	{base: UART0_BASE, rxbuf: &u0_rxbuf, txbuf: &u0_txbuf},
	{base: UART1_BASE, rxbuf: &u1_rxbuf, txbuf: &u1_txbuf},
};

/* register definitions */
enum uart_reg {
	RBR = 0x00 | REG_DLAB0, /* r */
	THR = 0x00 | REG_DLAB0, /* w */
	DLL = 0x00 | REG_DLAB1, /* rw */
	DLM = 0x04 | REG_DLAB1, /* rw */
	IER = 0x04 | REG_DLAB0, /* rw */
	IIR = 0x08, /* r */
	FCR = 0x08, /* w */
	LCR = 0x0c, /* rw */
	LSR = 0x14, /* r */
	SCR = 0x1c, /* rw */
	ACR = 0x20, /* rw */
	FDR = 0x28, /* rw */
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
	uint16_t div = (60000000 / (16 * baudrate));

	uart_reg_write(uart, LCR, LCR_WL_8|LCR_SB_1|LCR_PAR_NONE);

	uart_set_dlab(uart, 1);

	uart_reg_write(uart, DLL, div & 0xFF);
	uart_reg_write(uart, DLM, (div>>8) & 0xFF);

	uart_set_dlab(uart, 0);

	uart_reg_write(uart, FCR, FCR_ENABLE|FCR_RX_TRIG_1);

	fifo_init(&uarts[uart].rxfifo, uarts[uart].rxbuf);
	fifo_init(&uarts[uart].txfifo, uarts[uart].txbuf);

	uart_reg_write(uart, IER, IER_RBR);
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
	bool_t r;
	size_t done;
	uint32_t m;
	uint8_t c;
	uint8_t ier;

	m = disableIRQ();

	for(done = 0; done < nbytes; done++) {
		r = readb(UART_REG(uart, LSR)) & LSR_TEMT;
		c = ((uint8_t *)buf)[done];
		if(r) {
			writeb(c, UART_REG(uart, THR));
		} else {
			r = fifo_put(&uarts[uart].txfifo, c);

			if(r) {
				ier = uart_reg_read(uart, IER);
				ier |= IER_THRE;
				uart_reg_write(uart, IER, ier);
			} else {
				break;
			}
		}
	}

	restoreIRQ(m);

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
	uint8_t *cbuf = (uint8_t *)buf;
	size_t done;
	uint32_t m;

	m = disableIRQ();

	for(done = 0; done < nbytes; done++) {
		bool_t r = fifo_get(&uarts[uart].rxfifo, &cbuf[done]);
		if(!r) {
			break;
		}
	}

	restoreIRQ(m);

	return done;
}

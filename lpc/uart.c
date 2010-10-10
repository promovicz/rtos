
#include "uart.h"

#include "memory.h"

#include "armVIC.h"

#include "serial_fifo.h"

/* XXX find some better method for dealing with DLAB bullpoo */

#include "defines.h"

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

/* base address of all uarts and offset between uarts */
#define UART_BASE   0xe000c000
#define UART_OFFSET 0x4000

/* register address computation */
#define UART_REG(u,r) ((uint8_t*)(UART_BASE + (u * UART_OFFSET) + ((r) & ~(REG_FLAGS))))

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

fifo_t uart_rx_fifos[2];
char uart_rx_fifo_buf[VCOM_FIFO_SIZE][2];

fifo_t uart_tx_fifos[2];
char uart_tx_fifo_buf[VCOM_FIFO_SIZE][2];

extern int icount[2];

void uart_init(uart_t uart, uart_baud_t baudrate)
{
	icount[uart] = 0xF000000;

	uint16_t div = (60000000 / (16 * baudrate));

	uart_reg_write(uart, LCR, LCR_WL_8|LCR_SB_1|LCR_PAR_NONE);

	uart_set_dlab(uart, 1);

	uart_reg_write(uart, DLL, div & 0xFF);
	uart_reg_write(uart, DLM, (div>>8) & 0xFF);

	uart_set_dlab(uart, 0);

	uart_reg_write(uart, FCR, FCR_ENABLE|FCR_RX_TRIG_1);

	fifo_init(&uart_rx_fifos[uart], &uart_rx_fifo_buf[uart]);
	fifo_init(&uart_tx_fifos[uart], &uart_tx_fifo_buf[uart]);

	uart_reg_write(uart, IER, IER_RBR);
}

void uart_irq(uart_t uart)
{
	uint8_t ind = readb(UART_REG(uart, IIR));
	uint8_t w;

	icount[uart]++;

	if(ind&IIR_II_CTI) {
	}

	if(ind&IIR_II_RDA) {
		while(uart_rx_nonblocking(uart, &w)) {
			if(!fifo_put(&uart_rx_fifos[uart], w)) {
				break;
			}
		}
	}

	if(ind&IIR_II_THRE) {
		while (uart_reg_read(uart, LSR) & LSR_THRE) {
			if(!fifo_get(&uart_tx_fifos[uart], &w)) {
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

bool_t uart_rx_fifo(uart_t uart, uint8_t *c)
{
	uint32_t m;

	m = disableIRQ();

	bool_t r = fifo_get(&uart_rx_fifos[uart], c);

	restoreIRQ(m);

	return r;
}

void uart_tx_blocking(uart_t uart, uint8_t c)
{
	while (!(readb(UART_REG(uart, LSR)) & LSR_THRE)) { }
	writeb(c, UART_REG(uart, THR));
}

bool_t uart_tx_nonblocking(uart_t uart, uint8_t c)
{
	bool_t res = readb(UART_REG(uart, LSR)) & LSR_THRE;
	if(res) {
		writeb(c, UART_REG(uart, THR));
	}
	return res;
}

bool_t uart_tx_fifo(uart_t uart, uint8_t c)
{
	bool_t r;

	uint32_t m;
	m = disableIRQ();

	r = readb(UART_REG(uart, LSR)) & LSR_TEMT;

	if(r) {
		writeb(c, UART_REG(uart, THR));
	} else {
		r = fifo_put(&uart_tx_fifos[uart], c);

		if(r) {
			uint8_t m = uart_reg_read(uart, IER);
			m |= IER_THRE;
			uart_reg_write(uart, IER, m);
		}
	}

	restoreIRQ(m);

	return r;
}

void uart_rx_blocking(uart_t uart, uint8_t *c)
{
	while(!(readb(UART_REG(uart, LSR)) & LSR_RDR)) { }
	*c = readb(UART_REG(uart, RBR));
}

bool_t uart_rx_nonblocking(uart_t uart, uint8_t *c)
{
	bool_t res = readb(UART_REG(uart, LSR)) & LSR_RDR;
	if(res) {
		*c = readb(UART_REG(uart, RBR));
	}
	return res;
}

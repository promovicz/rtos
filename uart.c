
#include "uart.h"

#include "memory.h"

/* register flags used for latched register addressing */
#define REG_FLAGS  0xFF80
#define REG_DLAB   0x0080

/* base address of all uarts and offset between uarts */
#define UART_BASE   0xe000c000
#define UART_OFFSET 0x4000

/* register address computation */
#define UART_REG(u,r) ((uint8_t*)(UART_BASE + (u * UART_OFFSET) + ((r) & ~(REG_FLAGS))))

/* register definitions */
enum uart_reg {
	RBR = 0x00, /* r */
	THR = 0x00, /* w */
	DLL = 0x00 | REG_DLAB, /* rw */
	DLM = 0x04 | REG_DLAB, /* rw */
	IER = 0x04, /* rw */
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
static inline void uart_set_dlab(int uart, bool_t on)
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

/* configure DLAB for access to given register */
static inline void uart_configure_dlab(int uart, enum uart_reg r)
{
	static bool_t dlab_is_set = 0;

	if((r & REG_DLAB) && (!dlab_is_set)) {
		uart_set_dlab(uart, 1);
	} else if(!(r & REG_DLAB) && dlab_is_set) {
		uart_set_dlab(uart, 0);
	}
}

/* write given value to given uart register */
static inline void uart_reg_write(int uart, enum uart_reg r, uint8_t val)
{
	uart_configure_dlab(uart, r);

	writeb(val, UART_REG(uart, r));
}

/* read from given uart register */
static inline uint8_t uart_reg_read(int uart, enum uart_reg r)
{
	uart_configure_dlab(uart, r);

	return readb(UART_REG(uart, r));
}


void uart_init(int uart)
{
	uint16_t div = (60000000 / (16 * 115200));

	uart_reg_write(uart, LCR, LCR_WL_8|LCR_SB_1|LCR_PAR_NONE);
	uart_reg_write(uart, DLL, div & 0xFF);
	uart_reg_write(uart, DLM, (div>>8) & 0xFF);
	uart_reg_write(uart, FCR, FCR_ENABLE);

	uart_set_dlab(uart, 0);
}

void uart_tx_blocking(int uart, uint8_t c)
{
	while (!(readb(UART_REG(uart, LSR)) & LSR_THRE)) { }
	writeb(c, UART_REG(uart, THR));
}

bool_t uart_tx_nonblocking(int uart, uint8_t c)
{
	bool_t res = readb(UART_REG(uart, LSR)) & LSR_THRE;
	if(res) {
		writeb(c, UART_REG(uart, THR));
	}
	return res;
}

void uart_rx_blocking(int uart, uint8_t *c)
{
	while(!(readb(UART_REG(uart, LSR)) & LSR_RDR)) { }
	*c = readb(UART_REG(uart, RBR));
}

bool_t uart_rx_nonblocking(int uart, uint8_t *c)
{
	bool_t res = readb(UART_REG(uart, LSR)) & LSR_RDR;
	if(res) {
		*c = readb(UART_REG(uart, RBR));
	}
	return res;
}

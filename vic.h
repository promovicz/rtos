#ifndef VIC_H
#define VIC_H

#include "types.h"

typedef void (*vic_handler_t) (void);

void vic_enable(int src);
void vic_disable(int src);

void vic_default(vic_handler_t handler);

void vic_configure(int vector, int src, vic_handler_t handler);
void vic_deconfigure(int vector);

// XXX: LPC2148 specific
enum vic_int {
	INT_WDT = 0,
	INT_SWI,
	INT_DbgCommRx,
	INT_DbgCommTx,
	INT_TIMER0,
	INT_TIMER1,
	INT_UART0,
	INT_UART1,
	INT_PWM0,
	INT_I2C0,
	INT_SPI0,
	INT_SPI1,
	INT_PLL,
	INT_RTC,
	INT_EINT,
	INT_ADC0,
	INT_I2C1,
	INT_BOD,
	INT_ADC1,
	INT_USB,
	INT_COUNT
};

#if 0
// XXX: LPC2148 specific
char *vic_int_names[INT_COUNT] = {
	"WDT",
	"SWI",
	"DbgCommRx",
	"DbgCommTx",
	"TIMER0",
	"TIMER1",
	"UART0",
	"UART1",
	"PWM0",
	"I2C0",
	"SPI0",
	"SPI1",
	"PLL",
	"RTC",
	"EINT",
	"ADC0",
	"I2C1",
	"BOD",
	"ADC1",
	"USB"
};
#endif

#endif /* !VIC_H */

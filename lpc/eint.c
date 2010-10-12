
#include "eint.h"

struct eint_regs {
	uint8_t INT;
	uint8_t _pad0[3];
	uint16_t WAKE;
	uint8_t _pad1[2];
	uint8_t MODE;
	uint8_t _pad2[3];
	uint8_t POLAR;
	uint8_t _pad3[3];
};

enum eint_int_bits {
	EINT_INT_EINT0 = (1<<0),
	EINT_INT_EINT1 = (1<<1),
	EINT_INT_EINT2 = (1<<2),
	EINT_INT_EINT3 = (1<<3),
};

enum eint_wake_bits {
	EINT_WAKE_EXTWAKE0 = (1<<0),
	EINT_WAKE_EXTWAKE1 = (1<<1),
	EINT_WAKE_EXTWAKE2 = (1<<2),
	EINT_WAKE_EXTWAKE3 = (1<<3),

	EINT_WAKE_USBWAKE = (1<<5),

	EINT_WAKE_BODWAKE = (1<<14),
	EINT_WAKE_RTCWAKE = (1<<15),
};

#define EINT_BASE (0xE01FC140)
#define EINT ((volatile struct eint_regs*)EINT_BASE)

eint_handler_t handlers[NUM_EINT];

void eint_configure(int eint, bool_t edge, bool_t high_rising)
{
	uint8_t bit = 1 << eint;
	EINT->MODE = (EINT->MODE & ~bit) | ((edge?1:0) << eint);
	EINT->POLAR = (EINT->POLAR & ~bit) | ((high_rising?1:0) << eint);
}

void eint_handler(int eint, eint_handler_t handler)
{
	handlers[eint] = handler;
}

void eint_irq(int eint)
{
	if(handlers[eint]) {
		handlers[eint](eint);
	}
	EINT->INT |= 1 << eint;
}

#ifndef LPC_EINT_H
#define LPC_EINT_H

#include <core/types.h>

enum {
	EINT0 = 0,
	EINT1,
	EINT2,
	EINT3,

	NUM_EINT,
};

typedef void (*eint_handler_t)(int eint);

void eint_configure(int eint, bool_t edge, bool_t high_rising);
void eint_handler(int eint, eint_handler_t handler);
void eint_irq(int eint);

#endif /* !LPC_EINT_H */

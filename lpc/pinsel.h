#ifndef LPC_PINSEL_H
#define LPC_PINSEL_H

#include "pin.h"

int pin_set_function(int pin, int function);

bool_t pin_is_gpio(int pin);

void pin_report(void);

#endif /* !LPC_PINSEL_H */

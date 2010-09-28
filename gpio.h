#ifndef GPIO_H
#define GPIO_H

#include "types.h"

void gpio_direction(int port, int pin, bool_t output);

bool_t gpio_get(int port, int pin);

void gpio_clear(int port, int pin);
void gpio_set(int port, int pin);

#endif /* !GPIO_H */

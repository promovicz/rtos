#ifndef GPIO_H
#define GPIO_H

#include <core/types.h>

void gpio_pin_set_direction(int port, int pin, bool_t output);

bool_t gpio_pin_get_direction(int port, int pin);

bool_t gpio_pin_get(int port, int pin);

void gpio_pin_low(int port, int pin);
void gpio_pin_high(int port, int pin);

void gpio_pin_set(int port, int pin, bool_t state);

#endif /* !GPIO_H */

#ifndef GPIO_H
#define GPIO_H

#include <core/types.h>

void gpio_pin_set_direction(int pin, bool_t output);

bool_t gpio_pin_get_direction(int pin);

bool_t gpio_pin_get(int pin);

void gpio_pin_low(int pin);
void gpio_pin_high(int pin);

void gpio_pin_set(int pin, bool_t state);

#endif /* !GPIO_H */

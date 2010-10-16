
#include "gpio.h"

#include <lpc/pinsel.h>

struct gpio_regs {
	uint32_t PIN;
	uint32_t SET;
	uint32_t DIR;
	uint32_t CLR;
};

#define PORT0_BASE (0xE0028000)
#define PORT0 ((struct gpio_regs *)PORT0_BASE)

#define PORT1_BASE (0xE0028010)
#define PORT1 ((struct gpio_regs *)PORT1_BASE)

volatile struct gpio_regs *ports[] = {PORT0, PORT1};

void gpio_pin_set_direction(int port, int pin, bool_t output)
{
	if(output) {
		ports[port]->DIR |= (1<<pin);
	} else {
		ports[port]->DIR &= ~(1<<pin);
	}
}

bool_t gpio_pin_get_direction(int port, int pin)
{
	return ports[port]->DIR & (1<<pin) ? 1 : 0;
}

bool_t gpio_pin_get(int port, int pin)
{
	return ports[port]->PIN & (1<<pin) ? 1 : 0;
}

void gpio_pin_low(int port, int pin)
{
	ports[port]->CLR = (1<<pin);
}

void gpio_pin_high(int port, int pin)
{
	ports[port]->SET = (1<<pin);
}

void gpio_pin_set(int port, int pin, bool_t state)
{
	if(state) {
		gpio_pin_high(port, pin);
	} else {
		gpio_pin_low(port, pin);
	}
}

bool_t gpio_valid(int pin)
{
	if(pin >= 0 && pin < 32) { // XXX MAX
		return BOOL_TRUE;
	}
	return BOOL_FALSE;
}


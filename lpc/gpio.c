
#include "gpio.h"

#include <lpc/pinsel.h>

struct gpio_regs {
	uint32_t PIN;
	uint32_t SET;
	uint32_t DIR;
	uint32_t CLR;
};

#define PORT0_BASE (0xE0028000)
#define PORT0 ((volatile struct gpio_regs *)PORT0_BASE)

#define PORT1_BASE (0xE0028010)
#define PORT1 ((volatile struct gpio_regs *)PORT1_BASE)

volatile struct gpio_regs *ports[] = {PORT0, PORT1};

void gpio_pin_set_direction(int pin, bool_t output)
{
	int port = pin/32;
	if(output) {
		ports[port]->DIR |= (1<<pin);
	} else {
		ports[port]->DIR &= ~(1<<pin);
	}
}

bool_t gpio_pin_get_direction(int pin)
{
	int port = pin/32;
	return ports[port]->DIR & (1<<pin) ? 1 : 0;
}

bool_t gpio_pin_get(int pin)
{
	int port = pin/32;
	return ports[port]->PIN & (1<<pin) ? 1 : 0;
}

void gpio_pin_low(int pin)
{
	int port = pin/32;
	ports[port]->CLR = (1<<pin);
}

void gpio_pin_high(int pin)
{
	int port = pin/32;
	ports[port]->SET = (1<<pin);
}

void gpio_pin_set(int pin, bool_t state)
{
	int port = pin/32;
	if(state) {
		gpio_pin_high(pin);
	} else {
		gpio_pin_low(pin);
	}
}

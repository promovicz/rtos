
#include "gpio.h"

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

void gpio_direction(int port, int pin, bool_t output)
{
	if(output) {
		ports[port]->DIR |= (1<<pin);
	} else {
		ports[port]->DIR &= ~(1<<pin);
	}
}

void gpio_clear(int port, int pin)
{
	ports[port]->CLR = (1<<pin);
}

void gpio_set(int port, int pin)
{
	ports[port]->SET = (1<<pin);
}

bool_t gpio_get(int port, int pin)
{
	return ports[port]->PIN & (1<<pin);
}

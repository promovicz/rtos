
#include "pinsel.h"

#include <core/types.h>

#include <stdio.h>

struct pinsel_regs {
	uint32_t SELECT[2];
	uint32_t _pad0[3];
	uint32_t DBGTRACE;
};

enum dbgsel_bits {
	DBGSEL_ENABLE_DEBUG = (1<<2),
	DBGSEL_ENABLE_TRACE = (1<<3),
};

#define PINSEL_BASE (0xE002C000)
#define PINSEL ((volatile struct pinsel_regs *)PINSEL_BASE)

#define PIN_REGSEL(p) (p/16)
#define PIN_PINSEL(p) (p%16)

void pin_enable_debug(void)
{
	PINSEL->DBGTRACE |= DBGSEL_ENABLE_DEBUG;
}

int pin_set_function(int pin, int function)
{
	int i;
	int r = PIN_REGSEL(pin);
	int p = PIN_PINSEL(pin);
	const struct pin *pi = &pins[pin];

	for(i = 0; i < PIN_NUM_FUNCTIONS; i++) {
		if(pi->functions[i].function == function) {
			PINSEL->SELECT[r] = (PINSEL->SELECT[r] & ~(0x3<<(p*2))) | i<<(p*2);
			return 0;
		}
	}

	return -1;
}

int pin_get_function(int pin)
{
	int i;
	int r = PIN_REGSEL(pin);
	int p = PIN_PINSEL(pin);

	return (PINSEL->SELECT[r] >> p*2) & 0x3;
}

bool_t pin_is_gpio(int pin)
{
	int f = pin_get_function(pin);
	const struct pin *p = &pins[pin];
	return p->functions[0].device == DEVICE_GPIO && f == 0;
}

void pin_report(void)
{
	int i, j, c;
	const struct pin_function *f;
	printf("pin     functions\n");
	for(i = 0; i < NUM_PIN; i++) {
		uint8_t reg = i / 16;
		uint8_t pin = i % 16;
		uint8_t v = (PINSEL->SELECT[reg] >> (pin * 2)) & 0x03;
		const struct pin_function *f = &(pins[i].functions[v]);

		printf("%s\t*%s.%s", pins[i].name, device_name(f->device), f->name ? f->name : "reserved");

		c = 0;
		for(j = 0; j < PIN_NUM_FUNCTIONS; j++) {
			if(j != v && pins[i].functions[j].name) {
				c++;
			}
		}
		if(c) {
			for(j = 0; j < PIN_NUM_FUNCTIONS; j++) {
				f = &(pins[i].functions[j]);
				if(j != v && f->name) {
					printf(" %s.%s", device_name(f->device), f->name ? f->name : "reserved");
				}
			}
		}
		printf("\n");
	}
}

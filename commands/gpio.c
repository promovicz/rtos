
#include "commands.h"

#include <lpc/gpio.h>

void gpio_status(int pin)
{
	printf("gpio%d is %s state %s\n", pin,
		   gpio_pin_get_direction(pin)?"output":"input",
		   gpio_pin_get(pin)?"high":"low");
}

void gpio_status_all(void)
{
	int i;
	for(i = 0; i < 32; i++) { // XXX MAX
		if(pin_is_gpio(i)) {
			gpio_status(i);
		}
	}
}

void gpio_command(struct tty *t, int argc, char **argv)
{
	long int pin;
	if(argc) {
		if(!strcmp("status", argv[0])) {
			if(argc > 1) {
				if(scan_dec(argv[1], &pin)) {
					gpio_status(pin);
				}
			} else {
				gpio_status_all();
			}
		} else if (!strcmp("output", argv[0])) {
			if(argc > 1) {
				if(scan_dec(argv[1], &pin)) {
					gpio_pin_set_direction(pin, BOOL_TRUE);
				}
			}
		} else if (!strcmp("input", argv[0])) {
			if(argc > 1) {
				if(scan_dec(argv[1], &pin)) {
					gpio_pin_set_direction(pin, BOOL_FALSE);
				}
			}
		} else if (!strcmp("high", argv[0])) {
			if(argc > 1) {
				if(scan_dec(argv[1], &pin)) {
					gpio_pin_high(pin);
				}
			}
		} else if (!strcmp("low", argv[0])) {
			if(argc > 1) {
				if(scan_dec(argv[1], &pin)) {
					gpio_pin_low(pin);
				}
			}
		}
	} else {
		gpio_status_all();
	}
}

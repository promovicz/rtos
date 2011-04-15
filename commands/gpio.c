
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
		} else if (!strcmp("output", argv[0])) {
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

int command_gpio_status(struct cli *c, int argc, char **argv)
{
	long int pin;

	if(argc > 0) {
		if(scan_dec(argv[0], &pin)) {
			gpio_status(pin);
			return 0;
		}
	} else {
		gpio_status_all();
		return 0;
	}

	printf("Usage: gpio status [pin]\n");
	return 1;
}

int command_gpio_input(struct cli *c, int argc, char **argv)
{
	long int pin;

	if(argc > 0) {
		if(scan_dec(argv[0], &pin)) {
			gpio_pin_set_direction(pin, BOOL_TRUE);
			return 0;
		}
	}

   	printf("Usage: gpio input <pin>\n");
	return 1;
}

int command_gpio_output(struct cli *c, int argc, char **argv)
{
	long int pin;

	if(argc > 0) {
		if(scan_dec(argv[0], &pin)) {
			gpio_pin_set_direction(pin, BOOL_TRUE);
			return 0;
		}
	}

	printf("Usage: gpio output <pin>\n");
	return 1;
}

int command_gpio_high(struct cli *c, int argc, char **argv)
{
	long int pin;

	if(argc > 0) {
		if(scan_dec(argv[0], &pin)) {
			gpio_pin_high(pin);
			return 0;
		}
	}

	printf("Usage: gpio high <pin>\n");
	return 1;
}

int command_gpio_low(struct cli *c, int argc, char **argv)
{
	long int pin;

	if(argc > 0) {
		if(scan_dec(argv[0], &pin)) {
			gpio_pin_low(pin);
			return 0;
		}
	}

	printf("Usage: gpio low <pin>\n");
	return 1;
}

struct command cmd_gpio[] = {
	{.name = "status",
	 .help = "examine pin status",
	 .handler = &command_gpio_status},
	{.name = "input",
	 .help = "make pin an input",
	 .handler = &command_gpio_input},
	{.name = "output",
	 .help = "make pin an output",
	 .handler = &command_gpio_output},
	{.name = "high",
	 .help = "set pin to high",
	 .handler = &command_gpio_high},
	{.name = "low",
	 .help = "set pin to low",
	 .handler = &command_gpio_low},
};

DECLARE_COMMAND_TABLE(cmds_gpio, cmd_gpio);

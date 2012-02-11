
#include "commands.h"

#include <stdio.h>

#include <platform/lpc21/gpio.h>
#include <platform/lpc21/pinsel.h>

static void gpio_status(int pin)
{
	printf("gpio%d is %s state %s\n", pin,
		   gpio_pin_get_direction(pin)?"output":"input",
		   gpio_pin_get(pin)?"high":"low");
}

static void gpio_status_all(void)
{
	int i;
	for(i = 0; i < 32; i++) { // XXX MAX
		if(pin_is_gpio(i)) {
			gpio_status(i);
		}
	}
}

int command_gpio_status(maybe_unused struct cli *c, int argc, char **argv)
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

int command_gpio_input(maybe_unused struct cli *c, int argc, char **argv)
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

int command_gpio_output(maybe_unused struct cli *c, int argc, char **argv)
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

int command_gpio_high(maybe_unused struct cli *c, int argc, char **argv)
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

int command_gpio_low(maybe_unused struct cli *c, int argc, char **argv)
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
	{.name = "stat",
	 .help = "examine pin status",
	 .handler = &command_gpio_status},
	{.name = "in",
	 .help = "make pin an input",
	 .handler = &command_gpio_input},
	{.name = "out",
	 .help = "make pin an output",
	 .handler = &command_gpio_output},
	{.name = "hi",
	 .help = "set pin to high",
	 .handler = &command_gpio_high},
	{.name = "lo",
	 .help = "set pin to low",
	 .handler = &command_gpio_low},
};

DECLARE_COMMAND_TABLE(cmds_gpio, cmd_gpio);

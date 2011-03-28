
#include "commands.h"

#include <string.h>
#include <stdio.h>

#include <core/system.h>
#include <core/tick.h>

#include <lpc/reset.h>

const char const *resetnames[] = {
	"powerup",
	"external reset",
	"watchdog",
	"brownout detector",
	"unknown cause",
};

int command_system_status(struct cli *c, int argc, char **argv)
{
	tick_t tick;
	int rescause;

	printf("rtos version primordial\n");

	tick = tick_get();
	rescause = reset_cause();
	printf("running for %d.%03d secs, reset by %s\n", tick/1000, tick%1000, resetnames[rescause]);

	rtc_report();

	printf("watchdog is %s, reset %s\n",
		   wdt_enabled()?"enabled":"disabled",
		   wdt_reset_enabled()?"enabled":"disabled");

	return 0;
}

int command_system_reset(struct cli *c, int argc, char **argv)
{
	system_reset();
	return 0;
}

int command_system_halt(struct cli *c, int argc, char **argv)
{
	system_halt();
	return 0;
}

struct command cmd_system[] = {
	{"status", "report system status", &command_system_status, NULL},
	{"reset",  "reset the system",     &command_system_reset,  NULL},
	{"halt",   "halt the system",      &command_system_halt,   NULL},
	{NULL},
};

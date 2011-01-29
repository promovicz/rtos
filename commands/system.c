
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
	int r;
	tick_t tick;

	printf("rtos version primordial\n");

	r = reset_cause();
	printf("reset by %s\n", resetnames[r]);

	tick = tick_get();
	printf("sys time is %d.%03d\n", tick/1000, tick%1000);

	rtc_report();
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

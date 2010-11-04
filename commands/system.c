
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

void status_command(struct tty *t, int argc, char **argv)
{
	int r;
	tick_t tick;

	printf("rtos primordial running\n");

	r = reset_cause();
	printf("reset by %s\n", resetnames[r]);

	tick = tick_get();
	printf("sys time is %7d.%03d\n", tick/1000, tick%1000);

	rtc_report();
}

void reset_command(struct tty *t, int argc, char **argv)
{
	system_reset();
}

void halt_command(struct tty *t, int argc, char **argv)
{
	system_halt();
}


#include "commands.h"

#include <string.h>
#include <stdio.h>

#include <core/device.h>
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

int command_sys_stat(struct cli *c, int argc, char **argv)
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

int command_sys_time(struct cli *c, int argc, char **argv)
{
	tick_t now = tick_get();
	
	printf("system running for %d.%03d secs\n", now/1000, now%1000);

	timer_report();
}

int command_sys_dev(struct cli *c, int argc, char **argv)
{
	if(argc == 1) {
		struct device *dev = device_find(argv[0]);
		if(dev) {
			device_report(dev);
			return 0;
		} else {
			printf("Unknown device %s.", argv[0]);
			return 1;
		}
	}

	if(argc == 0) {
		device_report_all();
		return 0;
	}

	return 1;
}

int command_sys_reset(struct cli *c, int argc, char **argv)
{
	system_reset();
	return 0;
}

int command_sys_halt(struct cli *c, int argc, char **argv)
{
	system_halt();
	return 0;
}

struct command cmd_sys[] = {
	{.name = "stat",
	 .help = "report system status",
	 .handler = &command_sys_stat},
	{.name = "time",
	 .help = "report on timing",
	 .handler = &command_sys_time},
	{.name = "dev",
	 .help = "report device status",
	 .handler = &command_sys_dev},
	{.name = "reset", 
	 .help = "reset the system",
	 .handler = &command_sys_reset},
	{.name = "halt",
	 .help = "halt the system",
	 .handler = &command_sys_halt},
};

DECLARE_COMMAND_TABLE(cmds_sys, cmd_sys);

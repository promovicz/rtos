
#include "commands.h"

#include <sensor/nmea.h>

#include <stdio.h>
#include <string.h>

int command_nmea_status(struct cli *c, int argc, char **argv)
{
	printf("received %d sentences, %d framing errors, %d unknown sentences\n", nmea.sentences, nmea.errframing, nmea.errunknown);
	return 0;
}

int command_nmea_reset(struct cli *c, int argc, char **argv)
{
	printf("resetting nmea subsystem...");
	nmea_init();
	printf("done.");
	return 0;
}

struct command cmd_nmea[] = {
	{.name = "stat",
	 .help = "report nmea subsystem status",
	 .handler = &command_nmea_status},
	{.name = "reset",
	 .help = "reset nmea subsystem",
	 .handler = &command_nmea_reset},
};

DECLARE_COMMAND_TABLE(cmds_nmea, cmd_nmea);

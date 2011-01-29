
#include "commands.h"

#include <sensor/nmea.h>
#include <sensor/gps.h>

#include <stdio.h>
#include <string.h>

int command_nmea_reset(struct cli *c, int argc, char **argv)
{
	printf("Resetting NMEA receiver...");
	nmea_init();
	printf("done.");
	return 0;
}

int command_nmea_status(struct cli *c, int argc, char **argv)
{
	printf("encountered %d sentences, %d framing errors, %d unknown sentences\n", nmea.sentences, nmea.errframing, nmea.errunknown);
	return 0;
}

struct command cmd_nmea[] = {
	{"status", "report status of nmea receiver", &command_nmea_status},
	{"reset",  "reset nmea receiver",            &command_nmea_reset},
	{"",       NULL,                             &command_nmea_status},
	{NULL},
};

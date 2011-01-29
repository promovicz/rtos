
#include "commands.h"

#include <sensor/nmea.h>
#include <sensor/gps.h>

#include <stdio.h>
#include <string.h>

const char *gps_fixtype_str[] = {
	"None",
	"GPS SPS",
	"GPS DGPS",
	"GPS PPS",
	"RTK Fixed",
	"RTK Float",
	"Estimated",
	"Manual",
	"Simulator",
};

int command_gps_status(struct cli *c, int argc, char **argv)
{
	if(gps.fixvalid) {
		printf("fix at %d:%d:%d.%d type %d (%s)\n",
			   gps.fixhour, gps.fixminute, gps.fixsecond, gps.fixmilisecond,
			   gps.fixtype, gps_fixtype_str[gps.fixtype]);
		printf("  position latitude %02dd%02d.%04ds %c lon %03dd%02d.%04ds %c\n",
			   gps.fixlatmsb/100, gps.fixlatmsb%100, gps.fixlatlsb, gps.fixlathem,
			   gps.fixlonmsb/100, gps.fixlonmsb%100, gps.fixlonlsb, gps.fixlonhem);
	} else {
		printf("no fix\n");
	}

	if(gps.satsvalid && gps.numvissats) {
		printf("%d sats visible:\n", gps.numvissats);
		int i;
		for(i = 0; i < 12; i++) {
			struct gps_sat *sat = &gps.vissats[i];
			if(sat->satid) {
				printf("  id %d elev %d azim %d snr %d\n",
					   sat->satid, sat->satelev, sat->satazim, sat->satsnr);
			} else {
				break;
			}
		}
	} else {
		printf("no satellites\n");
	}

	return 0;
}

int command_gps_reset(struct cli *c, int argc, char **argv)
{
	printf("XXX\n");
	return 0;
}

struct command cmd_gps[] = {
	{"status", "report status of gps receiver", &command_gps_status},
	{"reset",  "reset gps receiver",            &command_gps_reset},
	{"",       NULL,                            &command_gps_status},
	{NULL},
};

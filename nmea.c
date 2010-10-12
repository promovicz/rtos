
#include "nmea.h"

#include <core/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NMEA_MAX 91
#define NMEA_MAX_ARGS 20

enum {
	STATE_IDLE,
	STATE_TYPE,
	STATE_ARGS,
	STATE_CSUM,
};

struct {
	int state;

	off_t fill;
	uint8_t argstart;
	uint8_t argfill;
	char sentence[NMEA_MAX+1];

	char *type;
	char *arguments[NMEA_MAX_ARGS];

	uint32_t sentences;
	uint32_t errframing;
	uint32_t errunknown;
} nmea;

struct gps_sat {
	int satid;
	int satelev;
	int satazim;
	int satsnr;
};

struct {
	bool_t fixvalid;
	int fixtype;
	int fixhour;
	int fixminute;
	int fixsecond;
	int fixmilisecond;

	bool_t satsvalid;
	int numvissats;
	struct gps_sat vissats[12];
} gps;


enum gps_fixtype {
	FIX_NONE = 0,
	FIX_GPS_SPS,
	FIX_GPS_DGPS,
	FIX_GPS_PPS,
	FIX_RTK_FIXED,
	FIX_RTK_FLOAT,
	FIX_ESTIMATED,
	FIX_MANUAL,
	FIX_SIMULATOR,
	_FIX_COUNT,
};

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

void
nmea_init() {
	nmea.state = STATE_IDLE;
	gps.fixvalid = BOOL_FALSE;
	gps.satsvalid = BOOL_FALSE;
}

void nmea_framing_error();
void nmea_unknown_sentence();

void
nmea_pusharg()
{
	nmea.arguments[nmea.argfill++] = &nmea.sentence[nmea.argstart];
	nmea.argstart = nmea.fill;
	if(nmea.argfill == NMEA_MAX_ARGS) {
		nmea_framing_error();
	}
}

void
nmea_push(char c)
{
	nmea.sentence[nmea.fill++] = c;
	if(nmea.fill == NMEA_MAX) {
		nmea_framing_error();
	}
}

bool_t
getargstr(int num, int min, int max, int *v)
{
	char *s = nmea.arguments[num];
	char *e;
	if(s && *s) {
		int r = strtol(nmea.arguments[num], &e, 10);
		if(!*e) {
			if(r >= min && r <= max) {
				*v = r;
				return BOOL_TRUE;
			}
		}
	}
	return BOOL_FALSE;
}

void nmea_report()
{
	printf("counters: %d sentences, %d framing errors, %d unknown sentences\n", nmea.sentences, nmea.errframing, nmea.errunknown);
	if(gps.fixvalid) {
		printf("fix at %d:%d:%d.%d type %d (%s)\n",
			   gps.fixhour, gps.fixminute, gps.fixsecond, gps.fixmilisecond,
			   gps.fixtype, gps_fixtype_str[gps.fixtype]);
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
	}
}

void nmea_command(struct tty *t, int argc, char **argv)
{
	if(argc > 0) {
		if(!strcmp(argv[1], "report")) {
			nmea_report();
		}
		if(!strcmp(argv[1], "reset")) {
			nmea_init();
		}
	} else {
		nmea_report();
	}
}

void nmea_process_gpgga()
{
	int r;
	char *e;
	int hour, minute, second, milisecond;

	r = sscanf(nmea.arguments[0], "%2d%2d%2d.%3d", &hour, &minute, &second, &milisecond);
	if(r != 4) {
		return;
	}

	int fixtype;
	if(!getargstr(5, 0, _FIX_COUNT-1, &fixtype)) {
		return;
	}

	gps.fixtype = fixtype;
	gps.fixhour = hour;
	gps.fixminute = minute;
	gps.fixsecond = second;
	gps.fixmilisecond = milisecond;
}

void nmea_process_gpgsv()
{
	int num_msg, cur_msg, num_sats;

	static int numsats;
	static struct gps_sat sats[12];

	if(getargstr(0, 1, 3, &num_msg)
	   && getargstr(1, 1, 3, &cur_msg)
	   && getargstr(2, 0, 12, &num_sats)) {

		if(cur_msg == 1) {
			numsats = 0;
			memset(&sats, 0, sizeof(sats));
		}

		int i;
		int arg = 3;
		for(i = 0; i < 4; i++) {
			int sid, elv, azi, snr;

			if(getargstr(arg + 0, 1, 64, &sid)
			   && getargstr(arg + 1, 0, 90, &elv)
			   && getargstr(arg + 2, 0, 359, &azi)
			   && getargstr(arg + 3, 0, 99, &snr)) {

				sats[numsats].satid   = sid;
				sats[numsats].satelev = elv;
				sats[numsats].satazim = azi;
				sats[numsats].satsnr  = snr;
				numsats++;
			}

			arg += 4;
		}

		if(cur_msg == num_msg) {
			gps.numvissats = numsats;
			memset(&gps.vissats, 0, sizeof(gps.vissats));
			memcpy(&gps.vissats, sats, sizeof(struct gps_sat)*numsats);
		}
	}
}

void
nmea_process_sentence()
{
	nmea.sentences++;

	if(!strcmp(nmea.type, "GPGGA")) {
		nmea_process_gpgga();
	}

	if(!strcmp(nmea.type, "GPGSV")) {
		nmea_process_gpgsv();
	}
}

void
nmea_process(const char *buf, size_t nbytes)
{
	size_t x;

	size_t p;

	for(p = 0; p < nbytes; p++) {
		char c = buf[p];

		switch(nmea.state) {
		case STATE_IDLE:
			if(c == '$') {
				nmea.state = STATE_TYPE;

				nmea.fill = 0;

				nmea_push(c);
			} // XXX else junk or gps-specific binary like on SUP500
			break;
		case STATE_TYPE:
			if(isalnum(c)) {
				nmea_push(c);
			} else if(c == ',') {
				nmea_push(0);

				nmea.type = &nmea.sentence[1];

				nmea.state = STATE_ARGS;
				nmea.argstart = nmea.fill;
				nmea.argfill = 0;
			} else {
				nmea_framing_error();
			}
			break;
		case STATE_ARGS:
			if(isalnum(c) || c == '.' || c == '-' || c == '+') {
				nmea_push(c);
			} else if(c == ',' || c == '*') {
				nmea_push(0);

				nmea_pusharg();

				if(c == '*') {
					nmea.state = STATE_CSUM;
				}
			} else {
				nmea_framing_error();
			}
			break;
		case STATE_CSUM:
			if(isxdigit(c)) {
				nmea_push(c);
			} else if (c=='\r') {
				// XXX process and verify checksum
				//     by computing incrementally while receiving.
				//     remember that saved sentence is modified
				//     for null-termination.
				nmea_process_sentence();
				nmea.state = STATE_IDLE;
			} else {
				nmea_framing_error();
			}
		}
	}
}

void
nmea_framing_error()
{
	nmea.errframing++;
	nmea.state = STATE_IDLE;
}

void
nmea_unknown_sentence()
{
	nmea.errunknown++;
	nmea.state = STATE_IDLE;
}


#include "nmea.h"

#include "types.h"

#include <sys/types.h>

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

} nmea;

void
nmea_init() {
	nmea.state = STATE_IDLE;
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

enum gps_fixtype_t {
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

char *gps_fixtype_str[] = {
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

struct {
	bool_t satvisible[12];
} gps;

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

void
nmea_process_sentence()
{
	int r;

	int fixtype;
	char *e;
	if(!strcmp(nmea.type, "GPGGA")) {

		int hour, minute, second, milisecond;
		r = sscanf(nmea.arguments[0], "%2d%2d%2d.%3d", &hour, &minute, &second, &milisecond);
		if(r == 4) {
			printf("time %d:%d:%d.%d\n", hour, minute, second, milisecond);
		}

		if(getargstr(5, 0, _FIX_COUNT-1, &fixtype)) {
			printf("fixtype %d - %s\n", fixtype, gps_fixtype_str[fixtype]);
		}

	}

	if(!strcmp(nmea.type, "GPGSV")) {
		int num_msg, cur_msg, num_sats;

		static int sats[12];
		static int sat_elev[12];
		static int sat_azim[12];
		static int sat_snr[12];

		if(getargstr(0, 1, 3, &num_msg)
		   && getargstr(1, 1, 3, &cur_msg)
		   && getargstr(2, 0, 12, &num_sats)) {

			printf("gsv %d/%d\n", cur_msg, num_msg);

			if(cur_msg == 1) {
				memset(&sats, 0, sizeof(sats));
			}

			int i;
			int arg = 3;
			int sat = (cur_msg - 1) * 4;
			for(i = 0; i < 4; i++) {
				int sid, elv, azi, snr;

				if(getargstr(arg + 0, 1, 64, &sid)
				   && getargstr(arg + 1, 0, 90, &elv)
				   && getargstr(arg + 2, 0, 359, &azi)
				   && getargstr(arg + 3, 0, 99, &snr)) {

					sats[sat+i] = sid;
					sat_elev[sat+i] = elv;
					sat_azim[sat+i] = azi;
					sat_snr[sat+i] = snr;

					printf("sat %d at %d/%d with snr %d\n", sid, elv, azi, snr);
				}

				arg += 4;
			}
		}
	}
}

void
nmea_process(const char *buf, size_t nbytes)
{
	size_t x;

	size_t p;

	for(p = 0; p < nbytes; p++) {
		char c = buf[p];

		//printf("chr %c\n", c);

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

				//printf("sentence %s\n", nmea.type);

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

				//printf("arg %s\n", nmea.arguments[nmea.argfill-1]);

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
#if 0
				printf("NMEA %s", nmea.type);
				for(x = 0; x < nmea.argfill; x++) {
					printf(" -%s-", nmea.arguments[x]);
				}
				printf("\n");
#endif
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
	puts("NMEA framing error");
	nmea.state = STATE_IDLE;
}

void
nmea_unknown_sentence()
{
	puts("NMEA unknown sentence");
	nmea.state = STATE_IDLE;
}

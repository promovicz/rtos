
#include "nmea.h"
#include "gps.h"

#include <core/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct nmea nmea;

struct gps gps;

void
nmea_init() {
	nmea.state = NMEA_STATE_IDLE;
	gps.fixvalid = BOOL_FALSE;
	gps.satsvalid = BOOL_FALSE;
}

void nmea_framing_error();
void nmea_unknown_sentence();

static void
nmea_pusharg()
{
	nmea.arguments[nmea.argfill++] = &nmea.sentence[nmea.argstart];
	nmea.argstart = nmea.fill;
	if(nmea.argfill == NMEA_MAX_ARGS) {
		nmea_framing_error();
	}
}

static void
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
	if(!getargstr(5, 0, _GPS_FIX_COUNT-1, &fixtype)) {
		return;
	}

	gps.fixtype = fixtype;
	gps.fixhour = hour;
	gps.fixminute = minute;
	gps.fixsecond = second;
	gps.fixmilisecond = milisecond;

	gps.fixvalid = BOOL_TRUE;
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
			gps.satsvalid = (numsats > 0);
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
		case NMEA_STATE_IDLE:
			if(c == '$') {
				nmea.state = NMEA_STATE_TYPE;

				nmea.fill = 0;

				nmea_push(c);
			} // XXX else junk or gps-specific binary like on SUP500
			break;
		case NMEA_STATE_TYPE:
			if(isalnum(c)) {
				nmea_push(c);
			} else if(c == ',') {
				nmea_push(0);

				nmea.type = &nmea.sentence[1];

				nmea.state = NMEA_STATE_ARGS;
				nmea.argstart = nmea.fill;
				nmea.argfill = 0;
			} else {
				nmea_framing_error();
			}
			break;
		case NMEA_STATE_ARGS:
			if(isalnum(c) || c == '.' || c == '-' || c == '+') {
				nmea_push(c);
			} else if(c == ',' || c == '*') {
				nmea_push(0);

				nmea_pusharg();

				if(c == '*') {
					nmea.state = NMEA_STATE_CSUM;
				}
			} else {
				nmea_framing_error();
			}
			break;
		case NMEA_STATE_CSUM:
			if(isxdigit(c)) {
				nmea_push(c);
			} else if (c=='\r') {
				// XXX process and verify checksum
				//     by computing incrementally while receiving.
				//     remember that saved sentence is modified
				//     for null-termination.
				nmea_process_sentence();
				nmea.state = NMEA_STATE_IDLE;
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
	nmea.state = NMEA_STATE_IDLE;
}

void
nmea_unknown_sentence()
{
	nmea.errunknown++;
	nmea.state = NMEA_STATE_IDLE;
}

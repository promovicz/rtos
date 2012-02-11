
#include "nmea.h"
#include "gps.h"

#include <core/clock.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <platform/lpc21/rtc.h>

struct nmea nmea;

struct gps gps;

void
nmea_init() {
	memset(&nmea, 0, sizeof(nmea));
	memset(&gps, 0, sizeof(gps));
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
nmea_push(char c, char r)
{
	nmea.raw[nmea.fill] = r;
	nmea.sentence[nmea.fill] = c;

	nmea.fill++;

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

	int latmsb, latlsb;
	char *lathem;

	r = sscanf(nmea.arguments[1], "%4d.%4d", &latmsb, &latlsb);
	if(r != 2) {
		printf("gga lat invalid: \"%s\"\n", nmea.arguments[1]);
		return;
	}

	lathem = nmea.arguments[2];
	if(strcmp(lathem, "N") && strcmp(lathem, "S")) {
		printf("gga lathem invalid: \"%s\"\n", nmea.arguments[2]);
		return;
	}

	//printf("lat %5d.%5d %s\n", latmsb, latlsb, lathem);

	int lonmsb, lonlsb;
	char *lonhem;

	r = sscanf(nmea.arguments[3], "%5d.%5d", &lonmsb, &lonlsb);
	if(r != 2) {
		printf("gga lon invalid: \"%s\"\n", nmea.arguments[3]);
		return;
	}

	lonhem = nmea.arguments[4];
	if(strcmp(lonhem, "E") && strcmp(lonhem, "W")) {
		printf("gga lonhem invalid: \"%s\"\n", lonhem);
		return;
	}

	//printf("lon %5d.%5d %s\n", lonmsb, lonlsb, lonhem);

	int fixtype;
	if(!getargstr(5, 0, _GPS_FIX_COUNT-1, &fixtype)) {
		return;
	}

	gps.fixtype = fixtype;
	gps.fixhour = hour;
	gps.fixminute = minute;
	gps.fixsecond = second;
	gps.fixmilisecond = milisecond;
	gps.fixlatmsb = latmsb;
	gps.fixlatlsb = latlsb;
	gps.fixlathem = lathem[0];
	gps.fixlonmsb = lonmsb;
	gps.fixlonlsb = lonlsb;
	gps.fixlonhem = lonhem[0];

	gps.fixvalid = BOOL_TRUE;

	/* XXX suboptimal, but a sane way of preventing bad time setting by gps */
	if(gps.numvissats >= 1) {
		rtc_indicate_time(RTC_SOURCE_GPS, gps.fixhour, gps.fixminute, gps.fixsecond);
	}
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
nmea_sentence_hook(nmea_sentence_hook_t hook)
{
	nmea.sentence_hook = hook;
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

	if(nmea.sentence_hook) {
		nmea.sentence_hook(nmea.raw, nmea.fill);
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

				nmea_push(c, c);
			} // XXX else junk or gps-specific binary like on SUP500
			break;
		case NMEA_STATE_TYPE:
			if(isalnum(c)) {
				nmea_push(c, c);
			} else if(c == ',') {
				nmea_push(0, c);

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
				nmea_push(c, c);
			} else if(c == ',' || c == '*') {
				nmea_push(0, c);

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
				nmea_push(c, c);
			} else if (c=='\r') {
				nmea_push(0, c);
				nmea.state = NMEA_STATE_CR;
			} else {
				nmea_framing_error();
			}
			break;
		case NMEA_STATE_CR:
			if(c=='\n') {
				nmea_push(0, '\n');
				nmea_push(0, 0);
				// XXX process and verify checksum
				//     by computing incrementally while receiving.
				//     remember that saved sentence is modified
				//     for null-termination.
				nmea.state = NMEA_STATE_PROCESS;
				nmea_process_sentence();
				nmea.state = NMEA_STATE_IDLE;
			} else {
				nmea_framing_error();
			}
			break;
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

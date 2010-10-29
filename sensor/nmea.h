#ifndef NMEA_H
#define NMEA_H

#include <core/types.h>

#include <sys/types.h>

#define NMEA_MAX 91
#define NMEA_MAX_ARGS 20

enum {
	NMEA_STATE_IDLE,
	NMEA_STATE_TYPE,
	NMEA_STATE_ARGS,
	NMEA_STATE_CSUM,
};

struct nmea {
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
};

extern struct nmea nmea;

void nmea_init(void);

void nmea_process(const char *buf, size_t nbytes);

#endif /* !NMEA_H */

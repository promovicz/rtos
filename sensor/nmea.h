#ifndef NMEA_H
#define NMEA_H

#include <core/types.h>

#include <sys/types.h>

#define NMEA_MAX 100
#define NMEA_MAX_ARGS 20

typedef void (*nmea_sentence_hook_t) (const char *raw_sentence, size_t nbytes);

enum {
	NMEA_STATE_IDLE,
	NMEA_STATE_TYPE,
	NMEA_STATE_ARGS,
	NMEA_STATE_CSUM,
	NMEA_STATE_CR,
	NMEA_STATE_PROCESS,
};

struct nmea {
	int state;

	nmea_sentence_hook_t sentence_hook;

	off_t fill;
	uint8_t argstart;
	uint8_t argfill;
	char sentence[NMEA_MAX+1];
	char raw[NMEA_MAX+1];

	char *type;
	char *arguments[NMEA_MAX_ARGS];

	uint32_t sentences;
	uint32_t errframing;
	uint32_t errunknown;
};

extern struct nmea nmea;

void nmea_init(void);

void nmea_process(const char *buf, size_t nbytes);

void nmea_sentence_hook(nmea_sentence_hook_t function);

#endif /* !NMEA_H */

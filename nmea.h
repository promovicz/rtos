#ifndef NMEA_H
#define NMEA_H

#include "cli.h"

void nmea_init(void);

void nmea_push(char c);

void nmea_report(void);

void nmea_command(struct tty *t, int argc, char **argv);

#endif /* !NMEA_H */

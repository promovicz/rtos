#ifndef COMMANDS_H
#define COMMANDS_H

#include <core/cli.h>
#include <core/tty.h>
#include <core/parse.h>

extern struct command cmd_system[];
extern struct command cmd_posix[];
extern struct command cmd_gpio[];
extern struct command cmd_nmea[];
extern struct command cmd_gps[];
extern struct command cmd_mem[];

extern struct command cmd_lpc[];

#endif /* !COMMANDS_H */

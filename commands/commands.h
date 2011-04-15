#ifndef COMMANDS_H
#define COMMANDS_H

#include <core/cli.h>
#include <core/tty.h>
#include <core/parse.h>

extern struct command_table cmds_sys;
extern struct command_table cmds_posix;
extern struct command_table cmds_gpio;
extern struct command_table cmds_nmea;
extern struct command_table cmds_gps;
extern struct command_table cmds_mem;

extern struct command_table cmds_lpc;

#endif /* !COMMANDS_H */

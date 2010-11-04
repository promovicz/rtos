#ifndef COMMANDS_H
#define COMMANDS_H

#include <core/tty.h>
#include <core/parse.h>


void status_command(struct tty *t, int argc, char **argv);
void reset_command(struct tty *t, int argc, char **argv);
void halt_command(struct tty *t, int argc, char **argv);
void time_command(struct tty *t, int argc, char **argv);

void posix_command(struct tty *t, int argc, char **argv);

void gpio_command(struct tty *t, int argc, char **argv);

void mem_command(struct tty *t, int argc, char **argv);

void pin_report(void);

void nmea_command(struct tty *t, int argc, char **argv);

#endif /* !COMMANDS_H */

#ifndef COMMANDS_H
#define COMMANDS_H

#include <core/tty.h>
#include <parse.h>

void gpio_command(struct tty *t, int argc, char **argv);

void mem_command(struct tty *t, int argc, char **argv);

void pin_report(void);

void power_command(struct tty *t, int argc, char **argv);

#endif /* !COMMANDS_H */

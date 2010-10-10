#ifndef CORE_TTY_H
#define CORE_TTY_H

#include <sys/types.h>

#define TTY_WIDTH  80
#define TTY_HEIGHT 25

struct tty;

typedef void (*tty_command_handler_t) (struct tty *t, int argc, char **argv);

struct tty {
	char  t_line[TTY_WIDTH];
	char  t_prompt[TTY_WIDTH];

	off_t t_posn;

	off_t t_start;
	off_t t_end;

	tty_command_handler_t t_command_handler;
};

void tty_init(struct tty *t);
void tty_reset(struct tty *t);

void tty_feed(struct tty *t, int c);

void tty_message(struct tty *t, const char *msg);

void tty_command_handler(struct tty *t, tty_command_handler_t handler);

#endif /* !CORE_TTY_H */

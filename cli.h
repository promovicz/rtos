#ifndef CLI_H
#define CLI_H

#include <sys/types.h>

#define TTY_WIDTH  80
#define TTY_HEIGHT 25

struct tty {
	char  t_line[TTY_WIDTH];
	char  t_prompt[TTY_WIDTH];

	off_t t_posn;

	off_t t_start;
	off_t t_end;
};

void tty_init(struct tty *t);
void tty_reset(struct tty *t);

void tty_feed(struct tty *t, int c);

void tty_message(struct tty *t, const char *msg);

#endif /* !CLI_H */

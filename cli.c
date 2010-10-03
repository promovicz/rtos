
#include "cli.h"

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "vcom.h"

#define CTRL(x)	(x&037)

void
tty_write(struct tty *t, const char *buf, size_t len)
{
	off_t i;
	for(i = 0; i < len; i++) {
		vcom_tx_fifo(buf[i]);
	}
	return len;
}

void
tty_writestr(struct tty *t, const char *str)
{
	tty_write(t, str, strlen(str));
}

void
tty_writechr(struct tty *t, char c)
{
	tty_write(t, &c, 1);
}

void
tty_setprompt(struct tty *t, char *p)
{
	size_t len = strlen(p);

	if(len < sizeof(t->t_prompt)-1) {
		memset(t->t_prompt, 0, sizeof(t->t_prompt));
		strncpy(t->t_prompt, p, len);
	}

}

void
tty_init(struct tty *t)
{
	tty_setprompt(t, "> ");
	tty_reset(t);
}

void
tty_redraw(struct tty *t)
{
	tty_writestr(t, "\r\x1b[K");
	tty_writestr(t, t->t_line);

	int i = t->t_end;
	while(i > t->t_posn) {
		tty_writechr(t, '\b');
		i--;
	}
}

void
tty_message(struct tty *t, const char *msg)
{
	tty_writestr(t, "\r\x1b[K");
	tty_writestr(t, msg);

	tty_redraw(t);
}

void
tty_reset(struct tty *t)
{
	memcpy(t->t_line, t->t_prompt, sizeof(t->t_line));

	t->t_start = strlen(t->t_prompt);
	t->t_end = t->t_start;
	t->t_posn = t->t_start;

	tty_redraw(t);
}

int
tty_insert_at(struct tty *t, int c, off_t p)
{
	int atend = 0;

	if(p == t->t_end) {
		atend = 1;
	}

	if(t->t_start <= p && p <= t->t_end) {
		if(t->t_end + 1 < (int)sizeof(t->t_line)) {
			memmove(&t->t_line[p+1], &t->t_line[p], t->t_end - p);
			t->t_line[p] = c;
			t->t_end += 1;

			if(atend) {
				tty_writechr(t, c);
			} else {
				tty_redraw(t);
			}

			return 1;
		}
	}

	return 0;
}

int
tty_remove_at(struct tty *t, off_t p)
{
	int atend = 0;

	if(p == t->t_end - 1) {
		atend = 1;
	}

	if(t->t_start <= p && p < t->t_end) {
		if(t->t_start <= (t->t_end - 1)) {
			memmove(&t->t_line[p], &t->t_line[p+1], (t->t_end - p - 1));
			t->t_end -= 1;
			t->t_line[t->t_end] = 0;

			if(atend) {
				tty_writestr(t, "\b \b");
			} else {
				tty_redraw(t);
			}

			return 1;
		}
	}

	return 0;
}

extern uint32_t systime;

void
docmd(struct tty *t, const char *cmd)
{
	char *tok[512];
	char buf[512];
	char *pos = buf;
	int toknum = 0;
	size_t len;

	memset(tok, 0, sizeof(tok));
	strncpy(buf, cmd, sizeof(buf)-1);

	while(*pos) {
		pos += strspn(pos, " \t\r\n");
		len = strcspn(pos, " \t\r\n");
		if(len) {
			tok[toknum++] = pos;
			pos += len;
			if(*pos) {
				*pos = 0;
				pos++;
			}
		} else {
			break;
		}
	}

	printf("\n");

	if(t->t_command_handler) {
		t->t_command_handler(t, toknum, tok);
	}
}

void
tty_feed(struct tty *t, int c)
{
	if(isgraph(c) || (c == ' ')) {
		if(tty_insert_at(t, c, t->t_posn)) {
			t->t_posn++;
			tty_redraw(t);
		}
	} else {
		switch(c) {
		case CTRL('l'):
			tty_redraw(t);
			break;
		case CTRL('c'):
			tty_reset(t);
			break;
		case CTRL('p'):
			if(t->t_posn > t->t_start) {
				t->t_posn--;
				tty_writestr(t, "\x1b[D");
			}
			break;
		case CTRL('n'):
			if(t->t_posn < t->t_end) {
				t->t_posn++;
				tty_writestr(t, "\x1b[C");
			}
			break;
		case CTRL('a'):
			t->t_posn = t->t_start;
			tty_redraw(t);
			break;
		case CTRL('e'):
			t->t_posn = t->t_end;
			tty_redraw(t);
			break;
		case CTRL('h'):
			if(tty_remove_at(t, t->t_posn - 1)) {
				t->t_posn--;
				tty_redraw(t);
			}
			break;
		case CTRL('m'):
			if(strlen(&t->t_line[t->t_start])) {
				docmd(t, &t->t_line[t->t_start]);
			} else {
				tty_writestr(t, "\r\n");
			}
			tty_reset(t);
			break;
		default:
			break;
		}
	}
}

void
tty_finish(struct tty *t)
{
	tty_writestr(t, "\r\n");
}

void
tty_command_handler(struct tty *t, tty_command_handler_t handler)
{
	t->t_command_handler = handler;
}

#if 0
int
main(void)
{
	struct tty t;

	struct termios o, r;
	cfmakeraw(&r);
	tcgetattr(0, &o);
	tcsetattr(0, TCSANOW, &r);

	int quit = 0;

	tty_init(&t);

	while(!quit) {
		int c = getchar();
		switch(c) {
		case CTRL('d'):
			quit = 1;
			break;
		default:
			tty_feed(&t, c);
			break;
		}
	}

	tty_finish(&t);

	tcsetattr(0, TCSANOW, &o);

	return 0;
}
#endif

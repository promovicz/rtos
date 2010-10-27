#ifndef POSIX_CONTROL_H
#define POSIX_CONTROL_H

#include <posix/file.h>

void console_init(void);
void console_enable(void);

int uart_open(int uart);

void memory_init(void);

#endif /* !POSIX_CONTROL_H */

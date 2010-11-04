#ifndef POSIX_CONTROL_H
#define POSIX_CONTROL_H

#include "file.h"
#include "memory.h"

void console_init(void);
void console_enable(void);

int uart_open(int uart);

#endif /* !POSIX_CONTROL_H */

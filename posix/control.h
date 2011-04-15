#ifndef POSIX_CONTROL_H
#define POSIX_CONTROL_H

#include <posix/common.h>

#include "file.h"
#include "memory.h"

void posix_init(void);

void posix_console_init(void);
void posix_console_enable(void);

void posix_memory_init(void);
void posix_memory_report(void);

int uart_open(int uart);

#endif /* !POSIX_CONTROL_H */

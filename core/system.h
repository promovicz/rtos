#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

#include <core/common.h>

void system_init(void);

void system_kick(void);

void system_idle(void);

void system_reset(void);
void system_halt(void);

#endif /* !CORE_SYSTEM_H */

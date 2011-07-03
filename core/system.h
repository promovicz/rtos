#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

#include <core/common.h>

void system_init(void);

nanosecs_t system_get_time (void);

void system_kick(void);
void system_idle(void);

void system_reset(void);
void system_halt(void);

void system_report(void);

#endif /* !CORE_SYSTEM_H */

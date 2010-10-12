#ifndef CORE_TICK_H
#define CORE_TICK_H

#include <core/types.h>

#define TICK_HZ (1000)

#define TICK_SECOND ((tick_t)1*TICK_HZ)

typedef uint32_t tick_t;

void   tick_init(void);

tick_t tick_get(void);

void   tick_delay(tick_t duration);

void   tick_handler(void);

#endif /* !CORE_TICK_H */

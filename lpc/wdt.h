#ifndef LPC_WDT_H
#define LPC_WDT_H

#include <core/types.h>

void wdt_init(bool_t reset);

void wdt_kick(void);

void wdt_reset(void);

#endif /* LPC_WDT_H */

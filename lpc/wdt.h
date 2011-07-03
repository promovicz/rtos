#ifndef LPC_WDT_H
#define LPC_WDT_H

#include <core/types.h>

void wdt_init(bool_t reset);

bool_t wdt_enabled(void);
bool_t wdt_reset_enabled(void);

void wdt_kick(void);

void wdt_reset(void);

#endif /* LPC_WDT_H */

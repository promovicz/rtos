#ifndef LPC_RESET_H
#define LPC_RESET_H

#include <core/types.h>

enum {
	RESET_CAUSE_POWERUP = 0,
	RESET_CAUSE_EXTRESET,
	RESET_CAUSE_WATCHDOG,
	RESET_CAUSE_BROWNOUT,
	RESET_CAUSE_UNKNOWN,
};

int reset_cause(void);

#endif /* !LPC_RESET_H */

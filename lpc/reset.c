
#include "reset.h"

struct reset_regs {
	uint8_t RSID;
	uint8_t _pad0[3];
};

enum reset_rsid_bits {
	RESET_RSID_POR = (1<<0),
	RESET_RSID_EXTR = (1<<1),
	RESET_RSID_WDTR = (1<<2),
	RESET_RSID_BODR = (1<<3),
};

#define RESET_BASE (0xE01FC180)
#define RESET ((volatile struct reset_regs*)RESET_BASE)

int reset_cause(void)
{
	uint8_t rsid = RESET->RSID;
	if(rsid&RESET_RSID_POR) {
		return RESET_CAUSE_POWERUP;
	}
	if(rsid&RESET_RSID_EXTR) {
		return RESET_CAUSE_EXTRESET;
	}
	if(rsid&RESET_RSID_WDTR) {
		return RESET_CAUSE_WATCHDOG;
	}
	if(rsid&RESET_RSID_BODR) {
		return RESET_CAUSE_BROWNOUT;
	}
	return RESET_CAUSE_UNKNOWN;
}

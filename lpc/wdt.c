
#include "wdt.h"

#include <stdio.h>

struct wdt_regs {
	uint8_t MOD;
	uint8_t _pad0[3];
	uint32_t TC;
	uint8_t FEED;
	uint8_t _pad1[3];
	uint32_t TV;
};

enum mod_bits {
	MOD_WDRESET = (1<<0),
	MOD_WDEN = (1<<1),
	MOD_WDTOF = (1<<2),
	MOD_WDINT = (1<<3),
};

enum feed_bits {
	FEED_A = 0xAA,
	FEED_B = 0x55,
	FEED_RESET = 0x00,
};

#define WDT_BASE (0xE0000000)
#define WDT ((volatile struct wdt_regs *)WDT_BASE)

static void wdt_feed(void)
{
	WDT->FEED = FEED_A;
	WDT->FEED = FEED_B;
}

void wdt_init(bool_t reset)
{
	WDT->TC = 60000000 / 4;
	WDT->MOD = MOD_WDEN | (reset?MOD_WDRESET:0);
	wdt_feed();
}

void wdt_kick(void)
{
	wdt_feed();
}

void wdt_reset(void)
{
	WDT->FEED = FEED_A;
	WDT->FEED = FEED_RESET;
}

bool_t wdt_enabled(void)
{
	return (WDT->MOD & MOD_WDEN)?BOOL_TRUE:BOOL_FALSE;
}

bool_t wdt_reset_enabled(void)
{
	return (WDT->MOD & MOD_WDRESET)?BOOL_TRUE:BOOL_FALSE;
}

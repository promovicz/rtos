
#include "system.h"

#include <board/logomatic.h>

#include <lpc/pcon.h>
#include <lpc/wdt.h>

#include <core/timer.h>

#include <posix/control.h>

volatile bool_t timestamp_flag;
volatile time_t timestamp_time;

void timestamp_function(always_unused struct timer *t, tick_t now)
{
	if(!timestamp_flag) {
		timestamp_flag = 1;
		timestamp_time = now;
	}
}

DEFINE_PERIODIC_TIMER(timestamp, timestamp_function, 60*TICK_SECOND);

void system_init(void)
{
	board_early_init();

	wdt_init(BOOL_TRUE);

	board_init();

	posix_init();

	START_PERIODIC_TIMER(timestamp);
}

void system_kick(void)
{
	wdt_kick();

	if(timestamp_flag) {
		tick_t now = tick_get();
		printf("\n[%8d.%03d] --- timestamp %8d.%03d ---\n",
			   now / 1000, now % 1000,
			   timestamp_time / 1000, timestamp_time % 1000);
		timestamp_flag = 0;
	}
}

void system_idle(void)
{
	pcon_idle();
}

void system_reset(void)
{
	wdt_reset();
	while(1) { }
}

void system_halt(void)
{
	pcon_power_down();
	while(1) { }
}

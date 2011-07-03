
#include "system.h"

#include <stdio.h>

#include <board/logomatic/board.h>

#include <lpc/pcon.h>
#include <lpc/wdt.h>

#include <core/irq.h>
#include <core/timer.h>
#include <core/clock.h>

#include <posix/control.h>

volatile bool_t     timestamp_flag;
volatile nanosecs_t timestamp_time;

void timestamp_function(always_unused struct timer *t, nanosecs_t now)
{
	if(!timestamp_flag) {
		timestamp_flag = 1;
		timestamp_time = now;
	}
}

DEFINE_PERIODIC_TIMER(timestamp, timestamp_function, 60*NANOSECS_SEC);

void system_init(void)
{
	board_early_init();

	clock_select();
	timer_select();

	wdt_init(BOOL_TRUE);

	irq_enable();

	board_init();

	posix_init();

	START_PERIODIC_TIMER(timestamp);
}

nanosecs_t system_get_time (void)
{
	if(current_system_clock) {
		return clock_get_time(current_system_clock);
	}
	return 0;
}

void system_kick(void)
{
	wdt_kick();

	if(timestamp_flag) {
		nanosecs_t now = system_get_time();
		printf("\n[%8lld.%09lld] --- timestamp %lld.%09lld ---\n",
			   now / NANOSECS_SEC,
			   now % NANOSECS_SEC,
			   timestamp_time / NANOSECS_SEC,
			   timestamp_time % NANOSECS_SEC);
		timestamp_flag = 0;
	}
}

nanosecs_t idle_min = 3600 * NANOSECS_SEC;
nanosecs_t idle_max = 0;

void system_idle(void)
{
	nanosecs_t start, end, diff;
	start = system_get_time();
	pcon_idle();
	end = system_get_time();
	diff = end - start;
	if(diff > idle_max) {
		idle_max = diff;
	}
	if(diff < idle_min) {
		idle_min = diff;
	}
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

void system_report(void)
{
	printf("sys idle min %lld.%09lld max %lld.%09lld\n",
		   idle_min / NANOSECS_SEC, idle_min % NANOSECS_SEC,
		   idle_max / NANOSECS_SEC, idle_max % NANOSECS_SEC);
}

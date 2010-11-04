
#include <board/logomatic.h>

#include <lpc/pcon.h>
#include <lpc/wdt.h>

#include <posix/control.h>

void system_init(void)
{
	board_init();

	memory_init();

	console_init();

	wdt_init(BOOL_TRUE);
}

void system_kick(void)
{
	wdt_kick();
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

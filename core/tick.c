
#include "tick.h"

#include <core/system.h>

volatile tick_t tickcounter;

void tick_init(void)
{
	tickcounter = 0;
}

tick_t tick_get(void)
{
	return tickcounter;
}

void tick_delay(tick_t duration)
{
	tick_t end = tickcounter + duration;
	while(tickcounter != end) { system_idle(); }
}

void tick_handler(void)
{
	tickcounter++;

	timer_drive();
}

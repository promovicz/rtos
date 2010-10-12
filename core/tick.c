
#include "tick.h"

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
	while(tickcounter != end) { }
}

void tick_handler(void)
{
	tickcounter++;
}

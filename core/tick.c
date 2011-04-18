
#include "tick.h"

#include <core/system.h>
#include <core/clock.h>

volatile tick_t tickcounter;

static nanosecs_t tick_clk_read(struct clock_device *clock)
{
	tick_t now = tick_get();
	return now * NANOSECS_MSEC;
}

struct clock_device clktick = {
	.dev = {.name = "clktick",
			.class = DEVICE_CLASS_CLOCK},
	.resolution = (1 * NANOSECS_MSEC), // XXX fixed
	.read = &tick_clk_read,
};

void tick_init(void)
{
	tickcounter = 0;

	device_add(&clktick.dev);
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
}

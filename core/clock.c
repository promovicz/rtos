
#include "clock.h"

#include <core/irq.h>
#include <core/system.h>

#include <stdio.h>

struct clock_device *current_system_clock = NULL;

static void clock_device_report(struct device *dev)
{
	struct clock_device *clock = container_of(dev, struct clock_device, dev);

	nanosecs_t raw = clock->read(clock);
	nanosecs_t res = clock->resolution;
	nanosecs_t rng = clock->range;

	if(current_system_clock == clock) {
		printf(" active system clock\n");
	}

	printf(" time %lld.%09lld secs\n",
		   raw / NANOSECS_SEC, raw % NANOSECS_SEC);
	printf(" range %lld.%09lld secs\n",
		   rng / NANOSECS_SEC, rng % NANOSECS_SEC);
	printf(" resolution %lld nsecs\n", res);
}

static void clock_propose(struct device *dev, void *cookie)
{
	struct clock_device *clock = container_of(dev, struct clock_device, dev);
	nanosecs_t prevsystime = 0;

	if(!dev->report_cb) {
		dev->report_cb = &clock_device_report;
	}

	if(current_system_clock) {
		if(clock->resolution >= current_system_clock->resolution) {
			return;
		} else {
			prevsystime = clock_get_time(current_system_clock);
		}
	}

	clock->previous_system_time = prevsystime;
	clock->previous_value = clock->read(clock);

	if(current_system_clock) {
		device_unuse(&current_system_clock->dev);
	}
	current_system_clock = clock;
	device_use(dev);
}

void clock_select(void)
{
	device_foreach_of_class(DEVICE_CLASS_CLOCK, NULL, &clock_propose);
}

nanosecs_t clock_get_time (struct clock_device *dev)
{
	uint32_t m;
	nanosecs_t value, offset, result;

	m = irq_disable();

	value = dev->read(dev);

	/* compute offset from previous timer value while handling overflow */
	if(value < dev->previous_value) {
		offset = (dev->range - dev->previous_value) + value;
		printf("rollover v %lld o %lld p %lld r %lld!\n",
			   value, offset, dev->previous_value, dev->range);
	} else {
		offset = value - dev->previous_value;
	}

	/* remember value for later offset computation */
	dev->previous_value = value;

	/* apply offset to previous system time */
	result = dev->previous_system_time + offset;

	/* and remember previous systime */
	dev->previous_system_time = result;

	irq_restore(m);

	return result;
}

void clock_delay(nanosecs_t delay)
{
	nanosecs_t end = clock_get_time(current_system_clock) + delay;
	while(clock_get_time(current_system_clock) < end) { }
}

void clock_report(void)
{
	if(current_system_clock) {
		nanosecs_t now = clock_get_time(current_system_clock);
		nanosecs_t res = current_system_clock->resolution;
		nanosecs_t rng = current_system_clock->range;
		printf("active clock is %s:\n", current_system_clock->dev.name);
		printf("  time %lld.%09lld secs\n",
			   now / NANOSECS_SEC,
			   now % NANOSECS_SEC);
		printf("  range %lld.%09lld secs\n",
			   rng / NANOSECS_SEC,
			   rng % NANOSECS_SEC);
		printf("  resolution %lld nsecs\n",
			   res);
	}
}

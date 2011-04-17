
#include "clock.h"

struct clock_device *the_system_clock = NULL;

static void clock_device_report(struct device *dev)
{
	struct clock_device *clock = container_of(dev, struct clock_device, dev);

	nanosecs_t now = clock->read(clock);
	nanosecs_t res = clock->resolution;

	if(the_system_clock == clock) {
		printf(" active system clock\n");
	}

	printf(" time at %lld.%09lld secs\n",
		   now / NANOSECS_SEC, now % NANOSECS_SEC);
	printf(" resolution is %lld nsecs\n", res);
}

static void clock_propose(struct device *dev, void *cookie)
{
	struct clock_device *clock = container_of(dev, struct clock_device, dev);

	if(!dev->report_cb) {
		dev->report_cb = &clock_device_report;
	}

	if(the_system_clock) {
		if(clock->resolution < the_system_clock->resolution) {
			the_system_clock = clock;
		}
	} else {
		the_system_clock = clock;
	}
}

void clock_select(void)
{
	device_foreach_of_class(DEVICE_CLASS_CLOCK, NULL, &clock_propose);
}

nanosecs_t clock_get_time (void)
{
	return the_system_clock->read(the_system_clock);
}

nanosecs_t clock_get_resolution(void)
{
	return the_system_clock->resolution;
}

void clock_report(void)
{
	printf("advanced system clock is %s\n", the_system_clock?"enabled":"disabled");
	if(the_system_clock) {
		nanosecs_t now = clock_get_time();
		nanosecs_t res = clock_get_resolution();
		printf("time source is device %s\n", the_system_clock->dev.name);
		printf("current time is %8lld.%09lld sec\n",
			   now / NANOSECS_SEC, now % NANOSECS_SEC);
		printf("current resolution is %9lld nsec\n",
			   res);
	}
}

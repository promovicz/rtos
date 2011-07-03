#ifndef CORE_CLOCK_H
#define CORE_CLOCK_H

#include <core/common.h>

#include <core/device.h>

/* clock devices, counters used for measuring time */
struct clock_device {
	struct device dev;
	nanosecs_t resolution;
	nanosecs_t range;
	nanosecs_t previous_value;
	nanosecs_t previous_system_time;
	nanosecs_t (*read) (struct clock_device *dev);
};

/* reference to the system clock */
struct clock_device *current_system_clock;

/* trigger clock selection */
void clock_select(void);

/* query specific clock */
nanosecs_t clock_get_time (struct clock_device *clock);

/* busy-wait based on system clock */
void clock_delay(nanosecs_t delay);

/* report status of system clock */
void clock_report(void);

#endif /* !CORE_CLOCK_H */

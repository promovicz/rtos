#ifndef CORE_CLOCK_H
#define CORE_CLOCK_H

#include <core/common.h>

#include <core/device.h>

struct clock_device {
	struct device dev;
	nanosecs_t resolution;
	nanosecs_t (*read) (struct clock_device *dev);
};

void clock_select(void);

nanosecs_t clock_get_time      (void);
nanosecs_t clock_get_resolution(void);

void clock_delay(nanosecs_t delay);

void clock_report(void);

#endif /* !CORE_CLOCK_H */

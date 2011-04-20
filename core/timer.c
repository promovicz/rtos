
#include "timer.h"

#include <core/clock.h>

#include <lpc/timer.h>

#include <stdio.h>

LLIST_HEAD(timer_queue);

struct timer_device *the_system_timer = NULL;

static void timer_adjust(nanosecs_t now);

static void timer_initialize (void) {
	the_system_timer->callback = &timer_drive;
	the_system_timer->initialize(the_system_timer);
	timer_adjust(clock_get_time());
}

static void timer_program (int mode, nanosecs_t delta) {
	the_system_timer->program(the_system_timer, mode, delta);
}

static void timer_shutdown(void) {
	timer_program(TIMER_MODE_DISABLED, 0);
}

static void timer_adjust(nanosecs_t now) {
	struct timer *entry;

	if(the_system_timer) {
		llist_for_each_entry(entry, &timer_queue, queue) {
			if(entry->enabled) {
				nanosecs_t delta = entry->expires - now;
				if(delta > 0) {
					if(delta < the_system_timer->min_delta) {
						delta = the_system_timer->min_delta;
					}
					if(delta > the_system_timer->max_delta) {
						delta = the_system_timer->max_delta;
					}

					timer_program(TIMER_MODE_ONESHOT, delta);

					return;
				}
			}
		}
	}
}

static void timer_propose(struct device *dev, void *cookie)
{
	struct timer_device *timer = container_of(dev, struct timer_device, dev);

	if(the_system_timer) {
		if(timer->min_delta < the_system_timer->min_delta) {
			timer_shutdown();
			the_system_timer = timer;
			timer_initialize();
		}
	} else {
		the_system_timer = timer;
		timer_initialize();
	}
}

void timer_select(void)
{
	device_foreach_of_class(DEVICE_CLASS_TIMER, NULL, &timer_propose);
	timer_adjust(clock_get_time());
}

void sleep_done(struct timer *t, nanosecs_t now)
{
}

DEFINE_TIMER(sleeper, sleep_done);

void timer_sleep(nanosecs_t duration)
{
	if(the_system_timer) {
		nanosecs_t start = clock_get_time();
		nanosecs_t progress;
		timer_add_relative(&sleeper, duration);
		do {
			system_idle();
			progress = clock_get_time() - start;
		} while (progress < duration);
	} else {
		clock_delay(duration);
	}
}


int timer_add(struct timer *t)
{
	struct timer *entry;

	if(t->enabled) {
		llist_del(&t->queue);
	}

	INIT_LLIST_HEAD(&t->queue);

	t->enabled = BOOL_TRUE;

	llist_for_each_entry(entry, &timer_queue, queue) {
		if(t->expires < entry->expires) {
			llist_add_tail(&t->queue, &entry->queue);
			return 0;
		}
	}

	llist_add_tail(&t->queue, &timer_queue);

	timer_adjust(clock_get_time());

	return 0;
}

int timer_add_absolute(struct timer *t, nanosecs_t time)
{
	t->expires = time;
	return timer_add(t);
}

int timer_add_relative(struct timer *t, nanosecs_t delay)
{
	t->expires = clock_get_time() + delay;
	return timer_add(t);
}

int timer_cancel(struct timer *t)
{
	if(t->enabled) {
		t->enabled = BOOL_FALSE;

		llist_del(&t->queue);

		timer_adjust(clock_get_time());
	}

	return 0;
}

void timer_drive(struct timer_device *dev)
{
	nanosecs_t now;
	struct timer *entry;

	now = clock_get_time();

 restart:
	llist_for_each_entry(entry, &timer_queue, queue) {
		if(now >= entry->expires) {
			llist_del(&entry->queue);
			if(entry->enabled) {
				entry->enabled = 0;
				if(entry->callback) {
					entry->callback(entry, now);
				}
			}
			goto restart;
		}
	}

	timer_adjust(now);
}

void timer_report(void)
{
	nanosecs_t now;
	struct timer *entry;

	now = clock_get_time();

	printf("timers driven by %s:\n",
		   the_system_timer ? the_system_timer->dev.name : "none");
	llist_for_each_entry(entry, &timer_queue, queue) {
		if(entry->enabled) {
			nanosecs_t rem = entry->expires - now;
			if(rem >= 0) {
				printf("  in %2lld.%09lld %s\n",
					   rem / NANOSECS_SEC,
					   rem % NANOSECS_SEC,
					   entry->name);
			}
		}
	}
}

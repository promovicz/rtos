
#include "timer.h"

#include <stdio.h>

LLIST_HEAD(timer_queue);

int timer_add(struct timer *t)
{
	struct timer *entry;

	INIT_LLIST_HEAD(&t->queue);

	t->enabled = BOOL_TRUE;

	llist_for_each_entry(entry, &timer_queue, queue) {
		if(t->expires < entry->expires) {
			llist_add_tail(&t->queue, &entry->queue);
			return 0;
		}
	}

	llist_add_tail(&t->queue, &timer_queue);
	return 0;
}

int timer_add_absolute(struct timer *t, tick_t time)
{
	t->expires = time;
	return timer_add(t);
}

int timer_add_relative(struct timer *t, tick_t delay)
{
	t->expires = tick_get() + delay;
	return timer_add(t);
}

int timer_cancel(struct timer *t)
{
	t->enabled = BOOL_FALSE;
	return 0;
}

void timer_drive(void)
{
	tick_t now;
	struct timer *entry;

	now = tick_get();

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
}

void timer_report(void)
{
	tick_t now;
	struct timer *entry;

	now = tick_get();

	printf("timers in queue:\n");

	llist_for_each_entry(entry, &timer_queue, queue) {
		tick_t rem = entry->expires - now;
		printf("  in %2d.%03d %s\n",
			   rem/1000, rem%1000, entry->name);
	}
}

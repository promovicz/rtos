#ifndef CORE_TIMER_H
#define CORE_TIMER_H

#include <core/common.h>
#include <core/tick.h>
#include <core/list.h>

/* forward declarations */
struct timer;

/* type for timer callback functions */
typedef void (*timer_cb_t) (struct timer *t, tick_t time);

/* a one-shot timer structure */
struct timer {
	char *name;
	timer_cb_t callback;
	bool_t enabled;
	tick_t expires;
	struct llist_head queue;
};

/* define a static timer calling the given callback */
#define DEFINE_TIMER(name, callback) \
	struct timer name = { 	         \
        .name: #name,                \
        .callback: &callback,	     \
	}

#define DEFINE_PERIODIC_TIMER(tname, tcallback, tperiod)		\
	static const tick_t tname ## _period = (tperiod);			\
	static void tname ## _cb (struct timer *t, tick_t now) {	\
		timer_add_relative(t, tname ## _period);				\
		tcallback(t, now);										\
	};															\
	struct timer tname = {										\
		.name = #tname,											\
		.callback = & tname ## _cb,								\
	};

#define START_PERIODIC_TIMER(tname) \
	timer_add_relative(&tname, tname ## _period)

#define STOP_PERIODIC_TIMER(tname) \
	timer_cancel(&tname)

int timer_add(struct timer *t);
int timer_add_absolute(struct timer *t, tick_t time);
int timer_add_relative(struct timer *t, tick_t delay);

int timer_cancel(struct timer *t);

void timer_drive(void);

#endif /* !CORE_TIMER_H */

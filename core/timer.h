#ifndef CORE_TIMER_H
#define CORE_TIMER_H

#include <core/common.h>
#include <core/device.h>
#include <core/clock.h>
#include <core/list.h>

/* forward declarations */
struct timer;

/* type for timer callback functions */
typedef void (*timer_cb_t) (struct timer *t, nanosecs_t time);

/* a one-shot timer structure */
struct timer {
	char *name;
	timer_cb_t callback;
	bool_t enabled;
	nanosecs_t expires;
	struct llist_head queue;
};

/* timer devices */
#define TIMER_FEAT_ONESHOT  (1<<0)
#define TIMER_FEAT_PERIODIC (1<<1)

#define TIMER_MODE_DISABLED (0)
#define TIMER_MODE_ONESHOT  (1)
#define TIMER_MODE_PERIODIC (2)

struct timer_device {
	struct device dev;
	unsigned features;
	nanosecs_t min_delta;
	nanosecs_t max_delta;
	int (*initialize) (struct timer_device *dev);
	int (*program) (struct timer_device *dev, int mode, nanosecs_t delta);
	void (*callback) (struct timer_device *dev);
};



/* define a static timer calling the given callback */
#define DEFINE_TIMER(tname, tcallback) \
	struct timer tname = {			   \
        .name = #tname,                \
        .callback = &tcallback,		   \
	}

#define DEFINE_PERIODIC_TIMER(tname, tcallback, tperiod)		 \
	static const nanosecs_t tname ## _period = (tperiod);		 \
	static void tname ## _cb (struct timer *t, nanosecs_t now) { \
		timer_add_relative(t, tname ## _period);				 \
		tcallback(t, now);										 \
	};															 \
	struct timer tname = {										 \
		.name = #tname,											 \
		.callback = & tname ## _cb,								 \
	};

#define START_PERIODIC_TIMER(tname) \
	timer_add_relative(&tname, tname ## _period)

#define STOP_PERIODIC_TIMER(tname) \
	timer_cancel(&tname)

void timer_select(void);

int timer_add(struct timer *t);
int timer_add_absolute(struct timer *t, nanosecs_t time);
int timer_add_relative(struct timer *t, nanosecs_t delay);

int timer_cancel(struct timer *t);

void timer_sleep(nanosecs_t delay);

void timer_drive(struct timer_device *dev);

#endif /* !CORE_TIMER_H */

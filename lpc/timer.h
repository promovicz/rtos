#ifndef TIMER_H
#define TIMER_H

#include <core/types.h>

/* TIMER CONFIG AND OPERATION */

enum {
	TIMER0 = 0,
	TIMER1 = 1,
};

void timer_init(int t);
void timer_reset(int t);
void timer_enable(int t, bool_t enable);
void timer_prescale(int t, uint32_t prescale);


/* MATCHING UNIT CONFIG AND OPERATION */

typedef enum {
	MR0 = 0,
	MR1 = 1,
	MR2 = 2,
	MR3 = 3,
} timer_match_t;

typedef enum {
	TIMER_MATCH_INTERRUPT = (1<<0),
	TIMER_MATCH_RESET     = (1<<1),
	TIMER_MATCH_STOP      = (1<<2),
} timer_match_action_t;

typedef void (*timer_match_handler_t) (int t, timer_match_t mr);

void timer_match_configure(int t, timer_match_t m,
						 uint32_t value,
						 timer_match_action_t actions);

void timer_match_handler(int t, timer_match_t m,
						 timer_match_handler_t handler);

void timer_irq(int t);

#define DEFINE_TIMER_VECTOR(vname, timer)			\
	static interrupt_handler void vname (void) {	\
		timer_irq(timer);							\
		vic_ack();									\
	};

#endif /* !TIMER_H */

#ifndef CORE_CLOCK_H
#define CORE_CLOCK_H

typedef uint32_t freq_t;

struct clock {
	/* parent clock */
	struct clock *parent;
};

#endif /* !CORE_CLOCK_H */

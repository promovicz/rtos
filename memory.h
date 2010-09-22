
#ifndef BIKE_MEMORY_H
#define BIKE_MEMORY_H

#include "defines.h"

static inline always_inline uint8_t readb(volatile uint8_t *a)
{
	return *a;
}

static inline always_inline void writeb(uint8_t v, volatile uint8_t *a)
{
	*a = v;
}

#endif /* !BIKE_MEMORY_H */

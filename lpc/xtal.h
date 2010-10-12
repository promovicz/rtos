#ifndef LPC_XTAL_H
#define LPC_XTAL_H

#include <core/defines.h>

static always_inline const uint32_t xtal_frequency(void)
{
	return 12000000; // XXX
}

#endif /* !LPC_XTAL_H */

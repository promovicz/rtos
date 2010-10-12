#ifndef LPC_PLL_H
#define LPC_PLL_H

#include <core/types.h>

#define THOUSAND (1000)
#define MILLION  (1000*THOUSAND)
#define BILLION  (1000*MILLION)

typedef uint32_t freq_t;

struct clock {
};

enum {
	PLL_CORE = 0,
	PLL_USB  = 1,
};

struct pll_configuration {
	freq_t freq;
	uint8_t mul;
	uint8_t div;
};

extern const struct pll_configuration pll_24mhz;
extern const struct pll_configuration pll_36mhz;
extern const struct pll_configuration pll_48mhz;
extern const struct pll_configuration pll_60mhz;

void pll_init(int pll, struct clock *clk);
void pll_configure(int pll, const struct pll_configuration *cfg);
void pll_lock_blocking(int pll);
void pll_connect_blocking(int pll);

#endif /* !LPC_PLL_H */


#include "pll.h"

#include <core/defines.h>

struct pll_regs {
	uint8_t CON;
	uint8_t _pad0[3];
	uint8_t CFG;
	uint8_t _pad1[3];
	uint16_t STAT;
	uint8_t _pad2[2];
	uint8_t FEED;
	uint8_t _pad3[3];
};

enum pll_con_bits {
	PLL_CON_PLLE = (1<<0),
	PLL_CON_PLLC = (1<<1),
};

static uint8_t pll_psel(uint8_t p) {
	uint8_t r = 0;
	do {
		r++;
		p >>= 1;
	} while(p);
	return r-1;
}

#define PLL_MSEL(m) (m-1)
#define PLL_PSEL(p) (pll_psel(p))

#define PLL_CFG(msel, psel) (((PLL_PSEL(psel)&0x3)<<5)|(PLL_MSEL(msel)&0x1F))

enum pll_stat_bits {
	PLL_STAT_PLLE  = (1<<8),
	PLL_STAT_PLLC  = (1<<9),
	PLL_STAT_PLOCK = (1<<10),
};

#define PLL_STAT_GET_MSEL(s) (s&0x1F)
#define PLL_STAT_GET_PSEL(s) ((s>>5)%0x3)

enum pll_feed_values {
	PLL_FEED0 = 0xAA,
	PLL_FEED1 = 0x55,
};

#define PLL0_BASE (0xE01FC080)
#define PLL0 ((volatile struct pll_regs*)PLL0_BASE)

#define PLL1_BASE (0xE01FC0A0)
#define PLL1 ((volatile struct pll_regs*)PLL1_BASE)

static volatile struct pll_regs * const plls[] = {PLL0, PLL1};


const struct pll_configuration pll_24mhz =
	{24*MILLION, 2, 4};

const struct pll_configuration pll_36mhz =
	{36*MILLION, 3, 2};

const struct pll_configuration pll_48mhz =
	{48*MILLION, 4, 2};

const struct pll_configuration pll_60mhz =
	{60*MILLION, 5, 2};


enum {
	PLL_OFF,
	PLL_STARTING,
	PLL_LOCKED,
	PLL_CONNECTED,
};

struct pll_cb {
	int state;
	freq_t freqoff;
	freq_t freq;
};

struct pll_cb pllcb[] = {
	{PLL_OFF, 12*MILLION, 0},
	{PLL_OFF, 12*MILLION, 0},
};

void pll_init(int pll, struct clock *clk)
{
	// register with clock
}

void pll_feed(int pll)
{
	plls[pll]->FEED = PLL_FEED0;
	plls[pll]->FEED = PLL_FEED1;
}

void pll_configure(int pll, const struct pll_configuration *cfg)
{
	if(cfg) {
		plls[pll]->CFG = PLL_CFG(cfg->mul, cfg->div);
	} else {
		plls[pll]->CFG = 0;
	}
	pll_feed(pll);
}

void pll_lock_blocking(int pll)
{
	plls[pll]->CON |= PLL_CON_PLLE;
	pll_feed(pll);
	while(!(plls[pll]->STAT & PLL_STAT_PLOCK)) { }
}

void pll_connect(int pll)
{
	plls[pll]->CON |= PLL_CON_PLLC;
	pll_feed(pll);
}

void pll_print(int pll)
{
	printf("pll %d cfg %x ctl %x stat %x\n", pll, plls[pll]->CFG, plls[pll]->CON, plls[pll]->STAT);
	printf("60 is %x\n", PLL_CFG(pll_60mhz.mul, pll_60mhz.div));
}

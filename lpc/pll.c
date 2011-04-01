
#include "pll.h"

#include <core/defines.h>

#include <lpc/xtal.h>

#include <stdio.h>

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

static uint8_t pll_encode_psel(uint8_t p) {
	uint8_t r = 0;
	do {
		r++;
		p >>= 1;
	} while(p);
	return r-1;
}

static uint8_t pll_decode_psel(uint8_t p) {
	switch(p) {
	case 0:
		return 1;
	case 1:
		return 2;
	case 2:
		return 4;
	case 3:
		return 8;
	default:
		return 0;
	}
}

#define PLL_MSEL(m) (m-1)
#define PLL_PSEL(p) (pll_encode_psel(p))

#define PLL_CFG(msel, psel) (((PLL_PSEL(psel)&0x3)<<5)|(PLL_MSEL(msel)&0x1F))

enum pll_stat_bits {
	PLL_STAT_PLLE  = (1<<8),
	PLL_STAT_PLLC  = (1<<9),
	PLL_STAT_PLOCK = (1<<10),
};

#define PLL_STAT_GET_MSEL(s) ((s&0x1F)+1)
#define PLL_STAT_GET_PSEL(s) (pll_decode_psel((s>>5)&0x3))

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

static void pll_print(int pll)
{
	uint16_t s = plls[pll]->STAT;

	uint8_t m = PLL_STAT_GET_MSEL(s);
	uint8_t d = PLL_STAT_GET_PSEL(s);

	uint32_t f = xtal_frequency() * m;

	const char *state = "disabled";
	if(s&PLL_STAT_PLLE) {
		state = "enabled";
		if(s&PLL_STAT_PLOCK) {
			state = "locked";
			if(s&PLL_STAT_PLLC) {
				state = "connected";
			}
		}
	}

	printf("pll %d %s, cco %d Hz (div %d), out %d Hz (mul %d)\n", pll, state, f*d, d, f, m);
}

void pll_report(void)
{
	pll_print(PLL_CORE);
	pll_print(PLL_USB);
}

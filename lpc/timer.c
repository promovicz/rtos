
#include "timer.h"

struct timer_regs {
	uint32_t IR;
	uint32_t TCR;
	uint32_t TC;
	uint32_t PR;
	uint32_t PC;
	uint32_t MCR;
	uint32_t MR[4];
	uint32_t CCR;
	uint32_t CR[4];
	uint32_t EMR;
	uint32_t CTCR;
};

enum ir_bits {
	IR_MR0 = (1<<0),
	IR_MR1 = (1<<1),
	IR_MR2 = (1<<2),
	IR_MR3 = (1<<3),
	IR_CR0 = (1<<4),
	IR_CR1 = (1<<5),
	IR_CR2 = (1<<6),
	IR_CR3 = (1<<7),
};

enum tcr_bits {
	TCR_COUNTER_ENABLE = (1<<0),
	TCR_COUNTER_RESET  = (1<<1),
};

enum mcr_bits {
	MCR_MR0_INTERRUPT = (1<<0),
	MCR_MR0_RESET = (1<<1),
	MCR_MR0_STOP = (1<<2),
	MCR_MR1_INTERRUPT = (1<<3),
	MCR_MR1_RESET = (1<<4),
	MCR_MR1_STOP = (1<<5),
	MCR_MR2_INTERRUPT = (1<<6),
	MCR_MR2_RESET = (1<<7),
	MCR_MR2_STOP = (1<<8),
	MCR_MR3_INTERRUPT = (1<<9),
	MCR_MR3_RESET = (1<<10),
	MCR_MR3_STOP = (1<<11),
};

enum ccr_bits {
	CCR_CAP0_RISING = (1<<0),
	CCR_CAP0_FALLING = (1<<1),
	CCR_CAP0_INTERRUPT = (1<<2),
	CCR_CAP1_RISING = (1<<3),
	CCR_CAP1_FALLING = (1<<4),
	CCR_CAP1_INTERRUPT = (1<<5),
	CCR_CAP2_RISING = (1<<6),
	CCR_CAP2_FALLING = (1<<7),
	CCR_CAP2_INTERRUPT = (1<<8),
	CCR_CAP3_RISING = (1<<9),
	CCR_CAP3_FALLING = (1<<10),
	CCR_CAP3_INTERRUPT = (1<<11),
};

enum emr_bits {
	EMR_EM0 = (1<<0),
	EMR_EM1 = (1<<0),
	EMR_EM2 = (1<<0),
	EMR_EM3 = (1<<0),

	EMR_EMC0_NOTHING = (0x00<<4),
	EMR_EMC0_CLEAR   = (0x01<<4),
	EMR_EMC0_SET     = (0x02<<4),
	EMR_EMC0_TOGGLE  = (0x03<<4),

	EMR_EMC1_NOTHING = (0x00<<6),
	EMR_EMC1_CLEAR   = (0x01<<6),
	EMR_EMC1_SET     = (0x02<<6),
	EMR_EMC1_TOGGLE  = (0x03<<6),

	EMR_EMC2_NOTHING = (0x00<<8),
	EMR_EMC2_CLEAR   = (0x01<<8),
	EMR_EMC2_SET     = (0x02<<8),
	EMR_EMC2_TOGGLE  = (0x03<<8),

	EMR_EMC3_NOTHING = (0x00<<10),
	EMR_EMC3_CLEAR   = (0x01<<10),
	EMR_EMC3_SET     = (0x02<<10),
	EMR_EMC3_TOGGLE  = (0x03<<10),
};

enum ctcr_bits {
	CTCR_MODE_TIMER = 0x00,
	CTCR_MODE_COUNT_RISING = 0x01,
	CTCR_MODE_COUNT_FALLING = 0x02,
	CTCR_MODE_COUNT_BOTH = 0x03,

	CTCR_INPUT_CAP0 = 0x00,
	CTCR_INPUT_CAP1 = 0x40,
	CTCR_INPUT_CAP2 = 0x80,
	CTCR_INPUT_CAP3 = 0xC0,
};

#define TIMER0_BASE (0xE0004000)
#define TIMER0_REGS ((volatile struct timer_regs*)TIMER0_BASE)

#define TIMER1_BASE (0xE0008000)
#define TIMER1_REGS ((volatile struct timer_regs*)TIMER1_BASE)


static struct timer_regs * const timers[] = {TIMER0_REGS, TIMER1_REGS};

static timer_match_handler_t timer_match_handlers[2][4] = {{0,0,0,0},{0,0,0,0}};


void timer_init(int t)
{
	// disable interrupts
	timers[t]->IR = 0;

	// reset counter
	timers[t]->TCR = TCR_COUNTER_RESET;
	timers[t]->TCR = 0;

	// select default mode
	timers[t]->CTCR = CTCR_MODE_TIMER;

	// reset special functions
	timers[t]->MCR = 0;
	timers[t]->CCR = 0;
	timers[t]->EMR = 0;

	// reset prescaler
	timers[t]->PR = 0;
	timers[t]->PC = 0;
}

void timer_enable(int t, bool_t enable)
{
	if(enable) {
		timers[t]->TCR = TCR_COUNTER_ENABLE;
	} else {
		timers[t]->TCR = 0;
	}
}

uint32_t timer_read(int t)
{
	return timers[t]->TC;
}

void timer_prescale(int t, uint32_t prescale)
{
	timers[t]->PR = prescale;
	timers[t]->PC = 0;
}

void timer_reset(int t)
{
	timers[t]->TCR |= TCR_COUNTER_RESET;
	timers[t]->TCR &= ~TCR_COUNTER_RESET;
}

void timer_match_configure(int t, timer_match_t mr, uint32_t value, timer_match_action_t flags)
{
	uint32_t mrf;
	timers[t]->MR[mr] = value;
	// XXX machine dependent bit voodoo
	if(flags) {
		mrf = flags<<(mr*3);
		timers[t]->MCR |= mrf;
	} else {
		mrf = 0x03<<(mr*3);
		timers[t]->MCR &= ~mrf;
	}
}

uint32_t timer_read_match_value(int t, timer_match_t mr)
{
	return timers[t]->MR[mr];
}

uint32_t timer_read_match_actions(int t, timer_match_t mr)
{
	uint8_t mshift = mr*3;
	return (timers[t]->MCR >> mshift) & 0x03;
}

void timer_match_handler(int t, timer_match_t m, timer_match_handler_t h)
{
	timer_match_handlers[t][m] = h;
}

void timer_irq_match(int t, timer_match_t m)
{
	if(timer_match_handlers[t][m]) {
		timer_match_handlers[t][m](t,m);
	}
}

void timer_irq(int t)
{
	uint32_t f = timers[t]->IR;

	if(f&IR_MR0) {
		timer_irq_match(t, 0);
	}
	if(f&IR_MR1) {
		timer_irq_match(t, 1);
	}
	if(f&IR_MR2) {
		timer_irq_match(t, 2);
	}
	if(f&IR_MR3) {
		timer_irq_match(t, 3);
	}

	if(f&IR_CR0) {
	}
	if(f&IR_CR1) {
	}
	if(f&IR_CR2) {
	}
	if(f&IR_CR3) {
	}

	timers[t]->IR = f;
}

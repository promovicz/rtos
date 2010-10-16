
#include "pcon.h"

struct pcon_regs {
	uint8_t PCON;
	uint8_t _pad0[3];
	uint32_t PCONP;
};

enum pcon_pcon_bits {
	PCON_IDL = (1<<0),
	PCON_PD = (1<<1),
	PCON_PDDISBOD = (1<<2),
	PCON_BODPDM = (1<<3),
	PCON_BOGD = (1<<4),
	PCON_BORD = (1<<5),
};

enum pcon_pconp_bits {
	PCONP_TIMER0 = (1<<1),
	PCONP_TIMER1 = (1<<2),
	PCONP_UART0 = (1<<3),
	PCONP_UART1 = (1<<4),
	PCONP_PWM0 = (1<<5),

	PCONP_I2C0 = (1<<7),
	PCONP_SPI0 = (1<<8),
	PCONP_RTC = (1<<9),
	PCONP_SPI1 = (1<<10),

	PCONP_AD0 = (1<<12),

	PCONP_I2C1 = (1<<19),
	PCONP_AD1 = (1<<20),

	PCONP_USB = (1<<31),
};

#define PCON_BASE (0xE01FC0C0)
#define PCON_REGS ((volatile struct pcon_regs*)PCON_BASE)

void pcon_power_down(void)
{
	PCON_REGS->PCON |= PCON_PD;
}

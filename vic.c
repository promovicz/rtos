
#include "vic.h"

struct vic_regs {
	uint32_t IRQStatus;
	uint32_t FIQStatus;
	uint32_t RawIntr;
	uint32_t IntSelect;
	uint32_t IntEnable;
	uint32_t IntEnableClear;
	uint32_t SoftInt;
	uint32_t SoftIntClear;
	uint32_t Protection;
	uint8_t  _pad0[12];
	vic_handler_t Vector;
	vic_handler_t DefVectAddr;
	uint8_t  _pad1[200];
	vic_handler_t VectAddr[16];
	uint8_t  _pad2[192];
	uint32_t VectCntl[16];
};

#define VIC_BASE (0xFFFFF000)
#define VIC ((struct vic_regs *)VIC_BASE)

#define INT_BIT(n) ((uint32_t)(1<<(n)))


// XXX: LPC2148 specific
char *vic_int_names[INT_COUNT] = {
	"WDT",
	"SWI",
	"DbgCommRx",
	"DbgCommTx",
	"TIMER0",
	"TIMER1",
	"UART0",
	"UART1",
	"PWM0",
	"I2C0",
	"SPI0",
	"SPI1",
	"PLL",
	"RTC",
	"EINT0",
	"EINT1",
	"EINT2",
	"EINT3",
	"ADC0",
	"I2C1",
	"BOD",
	"ADC1",
	"USB"
};

void vic_enable(int src)
{
	VIC->IntEnable |= INT_BIT(src);
}

void vic_disable(int src)
{
	VIC->IntEnableClear = INT_BIT(src);
}

void vic_fiq(int src, bool_t fiq)
{
	if(fiq) {
		VIC->IntSelect |= INT_BIT(src);
	} else {
		VIC->IntSelect &= ~INT_BIT(src);
	}
}

void vic_default(vic_handler_t handler)
{
	VIC->DefVectAddr = handler;
}

#define VIC_VECT_ENABLE (1<<5)
#define VIC_VECT_SOURCE(v) ((v)&0x1f)

#define VIC_VECT_CNTL(src, enable) \
	((uint32_t)((src)|((enable)?VIC_VECT_ENABLE:0)))


void vic_configure(int vector, int src, vic_handler_t handler)
{
	VIC->VectAddr[vector] = handler;
	VIC->VectCntl[vector] = VIC_VECT_CNTL(src, 1);
}

void vic_deconfigure(int vector)
{
	VIC->VectCntl[vector] = 0;
	VIC->VectAddr[vector] = 0;
}

void vic_report(void)
{
	int i;
	for(i = 0; i < INT_COUNT; i++) {
		uint32_t mask = (1<<i);
		if(VIC->IntEnable&mask) {
			printf("src %s enabled %s\n", vic_int_names[i],
					(VIC->IntSelect&mask)?"fiq":"");
		}
	}
	for(i = 0; i < 16; i++) {
		uint32_t cntl = VIC->VectCntl[i];
		if(cntl&VIC_VECT_ENABLE) {
			int src = VIC_VECT_SOURCE(cntl);
			printf("vec %d enabled src %s\n", i, vic_int_names[src]);
		}
	}
}

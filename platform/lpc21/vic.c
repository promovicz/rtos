
#include "vic.h"

#include <stdio.h>

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
	// XXX layout only valid for VECT_COUNT == 16
	vic_handler_t VectAddr[VIC_VECTOR_COUNT];
	uint8_t  _pad2[192];
	uint32_t VectCntl[VIC_VECTOR_COUNT];
};

#define VIC_BASE (0xFFFFF000)
#define VIC ((volatile struct vic_regs *)VIC_BASE)

#define INT_BIT(n) ((uint32_t)(1<<(n)))

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

void vic_ack(void)
{
	VIC->Vector = 0;
}

void vic_report(void)
{
	int i;
	for(i = 0; i < VIC_INT_COUNT; i++) {
		uint32_t mask = (1<<i);
		if(VIC->IntEnable&mask) {
			printf("src %s enabled type %s\n", vic_int_names[i],
					(VIC->IntSelect&mask)?"fiq":"irq");
		}
	}
	for(i = 0; i < VIC_VECTOR_COUNT; i++) {
		uint32_t cntl = VIC->VectCntl[i];
		if(cntl&VIC_VECT_ENABLE) {
			int src = VIC_VECT_SOURCE(cntl);
			printf("vec %d enabled src %s\n", i, vic_int_names[src]);
		}
	}
}

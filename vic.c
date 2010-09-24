
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


void vic_enable(int src)
{
	VIC->IntEnable |= INT_BIT(src);
	printf("ie @%x now %x, set %x\n", &VIC->IntEnable, VIC->IntEnable, INT_BIT(src));
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

#define VIC_VECT_CNTL(src, enable) \
	((uint32_t)((src)|((enable)?(1<<5):0)))

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



#if 0
#include <stdio.h>

#define print_offsetof(st, m) \
	printf("member %s is at offset 0x%lx\n", #m, __builtin_offsetof(st, m));

int
main(void) {
	print_offsetof(struct vic_regs, IRQStatus);
	print_offsetof(struct vic_regs, FIQStatus);
	print_offsetof(struct vic_regs, RawIntr);
	print_offsetof(struct vic_regs, IntSelect);
	print_offsetof(struct vic_regs, IntEnable);
	print_offsetof(struct vic_regs, IntEnableClear);
	print_offsetof(struct vic_regs, SoftInt);
	print_offsetof(struct vic_regs, SoftIntClear);
	print_offsetof(struct vic_regs, Protection);
	print_offsetof(struct vic_regs, Vector);
	print_offsetof(struct vic_regs, DefVectAddr);
	print_offsetof(struct vic_regs, VectAddr);
	print_offsetof(struct vic_regs, VectCntl);
	return 0;
}

#endif

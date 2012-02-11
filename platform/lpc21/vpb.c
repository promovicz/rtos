
#include "vpb.h"

#include <core/types.h>

struct vpb_regs {
	uint8_t DIV;
	uint8_t _pad0[3];
};

enum vpb_div_bits {
	VPB_DIV_4 = 0x00,
	VPB_DIV_1 = 0x01,
	VPB_DIV_2 = 0x02,
};

#define VPB_BASE (0xE01FC100)
#define VPB ((volatile struct vpb_regs*)VPB_BASE)

void vpb_init(void)
{
	VPB->DIV = VPB_DIV_1;
}

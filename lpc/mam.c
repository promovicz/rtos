
#include <core/types.h>

struct mam_regs {
	uint8_t CR;
	uint8_t _pad0[3];
	uint8_t TIM;
	uint8_t _pad1[3];
};

enum mam_cr_bits {
	MAM_MODE_DISABLED = 0x00,
	MAM_MODE_PARTIAL = 0x01,
	MAM_MODE_FULL = 0x02,
};

enum mam_tim_bits {
	MAM_TIME_1CCLK = 0x01,
	MAM_TIME_2CCLK = 0x02,
	MAM_TIME_3CCLK = 0x03,
	MAM_TIME_4CCLK = 0x04,
	MAM_TIME_5CCLK = 0x05,
	MAM_TIME_6CCLK = 0x06,
	MAM_TIME_7CCLK = 0x07,
};

#define MAM_BASE (0xE01FC000)
#define MAM ((volatile struct mam_regs*)MAM_BASE)

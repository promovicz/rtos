
#include <core/types.h>



struct other_regs {
	uint32_t SCS;
};

enum other_scs_bits {
	SCS_GPIO0_MODE_VPB = 0x00,
	SCS_GPIO0_MODE_HS = 0x01,
	SCS_GPIO1_MODE_VPB = 0x00,
	SCS_GPIO1_MODE_HS = 0x02,
};



struct mem_regs {
	uint8_t MAP;
	uint8_t _pad0[3];
};

enum mem_map_bits {
	MEM_MAP_BOOT = 0x00,
	MEM_MAP_USER_FLASH = 0x01,
	MEM_MAP_USER_RAM = 0x02,
};

#define MEM_BASE (0xE01FC040)
#define MEM ((volatile struct mem_regs*)MEM_BASE)



struct reset_regs {
	uint8_t RSID;
	uint8_t _pad0[3];
};

enum reset_rsid_bits {
	RESET_RSID_POR = (1<<0),
	RESET_RSID_EXTR = (1<<1),
	RESET_RSID_WDTR = (1<<2),
	RESET_RSID_BODR = (1<<3),
};

#define RESET_BASE (0xE01FC180)
#define RESET ((volatile struct reset_regs*)RESET_BASE)



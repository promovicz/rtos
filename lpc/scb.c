
#include <core/types.h>

struct eint_regs {
	uint8_t INT;
	uint8_t _pad0[3];
	uint16_t WAKE;
	uint8_t _pad1[2];
	uint8_t MODE;
	uint8_t _pad2[3];
	uint8_t POLAR;
	uint8_t _pad3[3];
};

enum eint_int_bits {
	EINT_INT_EINT0 = (1<<0),
	EINT_INT_EINT1 = (1<<1),
	EINT_INT_EINT2 = (1<<2),
	EINT_INT_EINT3 = (1<<3),
};

enum eint_wake_bits {
	EINT_WAKE_EXTWAKE0 = (1<<0),
	EINT_WAKE_EXTWAKE1 = (1<<1),
	EINT_WAKE_EXTWAKE2 = (1<<2),
	EINT_WAKE_EXTWAKE3 = (1<<3),

	EINT_WAKE_USBWAKE = (1<<5),

	EINT_WAKE_BODWAKE = (1<<14),
	EINT_WAKE_RTCWAKE = (1<<15),
};

enum eint_mode_bits {
	EINT_MODE_EXT0_LEVEL = 0x00,
	EINT_MODE_EXT0_EDGE = 0x01,

	EINT_MODE_EXT1_LEVEL = 0x00,
	EINT_MODE_EXT1_EDGE = 0x02,

	EINT_MODE_EXT2_LEVEL = 0x00,
	EINT_MODE_EXT2_EDGE = 0x04,

	EINT_MODE_EXT3_LEVEL = 0x00,
	EINT_MODE_EXT3_EDGE = 0x08,
};

enum eint_polar_bits {
	EINT_POLAR_EXT0_LOW = 0x00,
	EINT_POLAR_EXT0_FALLING = 0x00,
	EINT_POLAR_EXT0_HIGH = 0x01,
	EINT_POLAR_EXT0_RISING = 0x01,

	EINT_POLAR_EXT1_LOW = 0x00,
	EINT_POLAR_EXT1_FALLING = 0x00,
	EINT_POLAR_EXT1_HIGH = 0x02,
	EINT_POLAR_EXT1_RISING = 0x02,

	EINT_POLAR_EXT2_LOW = 0x00,
	EINT_POLAR_EXT2_FALLING = 0x00,
	EINT_POLAR_EXT2_HIGH = 0x04,
	EINT_POLAR_EXT2_RISING = 0x04,

	EINT_POLAR_EXT3_LOW = 0x00,
	EINT_POLAR_EXT3_FALLING = 0x00,
	EINT_POLAR_EXT3_HIGH = 0x08,
	EINT_POLAR_EXT3_RISING = 0x08,
};

#define EINT_BASE (0xE01FC140)
#define EINT ((volatile struct eint_regs*)EINT_BASE)



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
#define PCON ((volatile struct pcon_regs*)PCON_BASE)


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



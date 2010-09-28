
struct spi_regs {
	uint16_t CR;
	uint8_t _pad0[2];
	uint8_t SR;
	uint8_t _pad1[3];
	uint16_t DR;
	uint8_t _pad2[2];
	uint8_t CCR;
	uint8_t _pad3[3];
	uint8_t INT;
};

enum spi_cr_bits {
	CR_BIT_ENABLE = (1<<2),
	CR_CPHA = (1<<3),
	CR_CPOL = (1<<4),
	CR_MSTR = (1<<5),
	CR_LSBF = (1<<6),
	CR_SPIE = (1<<7),
	// XXX bit sizes
};

enum spi_sr_bits {
	SR_ABRT = (1<<3),
	SR_MODF = (1<<4),
	SR_ROVR = (1<<5),
	SR_WCOL = (1<<6),
	SR_SPIF = (1<<7)
};

enum spi_int_bits {
	INT_FLAG = (1<<0),
};

void spi_init(void)
{

}

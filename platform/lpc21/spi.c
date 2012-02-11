
#include "spi.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <core/parse.h>

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

#define SPI_BASE (0xE0020000)
#define SPI ((volatile struct spi_regs *)SPI_BASE)

#define spi_trace(...) if(spi.trace) { printf(__VA_ARGS__); }

struct {
	bool_t trace;
} spi;

void spi_init(void)
{
	SPI->CR = CR_CPHA|CR_CPOL|CR_MSTR;
	SPI->CCR = 150;
}

uint8_t spi_transfer(uint8_t t)
{
	uint8_t r;

	SPI->DR = t;

	while(!(SPI->SR & SR_SPIF)) {}

	r = SPI->DR;

	spi_trace(" %02x -> %02x\n", t, r);

	return r;
}

void spi_enable_trace(bool_t enable)
{
	spi.trace = enable ? BOOL_TRUE : BOOL_FALSE;
}

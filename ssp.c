
#include "ssp.h"

#include "hal.h"

#define WAIT_TXF_NOTFULL() do { while (!(SSP->SR & SR_TNF)); } while (0)
#define WAIT_RXF_NOTEMPTY() do { while (!(SSP->SR & SR_RNE)); } while (0)
#define WAIT_IDLE() do { while (SSP->SR & SR_BSY); } while (0)

struct ssp_regs {
	uint16_t CR0;
	uint8_t _pad0[2];
	uint8_t CR1;
	uint8_t _pad1[3];
	uint16_t DR;
	uint8_t _pad2[2];
	uint8_t SR;
	uint8_t _pad3[3];
	uint8_t CPSR;
	uint8_t _pad4[3];
	uint8_t IMSC;
	uint8_t _pad5[3];
	uint8_t RIS;
	uint8_t _pad6[3];
	uint8_t MIS;
	uint8_t _pad7[3];
	uint8_t ICR;
	uint8_t _pad8[3];
};

#define SSP_BASE (0xE0068000)
#define SSP ((struct ssp_regs *)SSP_BASE)

enum ssp_cr0_bits {
	CR0_DSS_4BIT  = 0x0003,
	CR0_DSS_5BIT  = 0x0004,
	CR0_DSS_6BIT  = 0x0005,
	CR0_DSS_7BIT  = 0x0006,
	CR0_DSS_8BIT  = 0x0007,
	CR0_DSS_9BIT  = 0x0008,
	CR0_DSS_10BIT = 0x0009,
	CR0_DSS_11BIT = 0x000A,
	CR0_DSS_12BIT = 0x000B,
	CR0_DSS_13BIT = 0x000C,
	CR0_DSS_14BIT = 0x000D,
	CR0_DSS_15BIT = 0x000E,
	CR0_DSS_16BIT = 0x000F,

	CR0_FRF_SPI   = 0x0000,
	CR0_FRF_SSI   = 0x0010,
	CR0_FRF_MICROWIRE = 0x0020,

	CR0_CPOL_AWAY = (0<<6),
	CR0_CPOL_BACK = (1<<6),

	CR0_CPHA_LOW = (0<<7),
	CR0_CPHA_HIGH = (1<<7),
};

#define CR0_SCR(v) ((v<<8)&0xFF00)

enum ssp_cr1_bits {
	CR1_LBM = (1<<0),
	CR1_SSE = (1<<1),
	CR1_MS  = (1<<2),
	CR1_SOD = (1<<3),
};

enum ssp_sr_bits {
	SR_TFE = (1<<0),
	SR_TNF = (1<<1),
	SR_RNE = (1<<2),
	SR_RFF = (1<<3),
	SR_BSY = (1<<4),
};

enum ssp_imsc_bits {
	IMSC_RORIM = (1<<0),
	IMSC_RTIM  = (1<<1),
	IMSC_RXIM  = (1<<2),
	IMSC_TXIM  = (1<<3),
};

enum ssp_ris_bits {
	RIS_RORRIS = (1<<0),
	RIS_RTRIS  = (1<<1),
	RIS_RXRIS  = (1<<2),
	RIS_TXRIS  = (1<<3),
};

enum ssp_mis_bits {
	MIS_RORMIS = (1<<0),
	MIS_RTMIS  = (1<<1),
	MIS_RXMIS  = (1<<2),
	MIS_TXMIS  = (1<<3),
};

enum ssp_icr_bits {
	ICR_RORIC = (1<<0),
	ICR_RTIC  = (1<<1),
};

void ssp_init(void)
{
	SSP->CR0 = CR0_DSS_8BIT | CR0_FRF_SPI;
	SSP->CR1 = 0;
	SSP->IMSC = 0;
}

void ssp_enable(bool_t enable)
{
	if(enable) {
		SSP->CR1 |= CR1_SSE;
	} else {
		SSP->CR1 &= ~CR1_SSE;
	}
}

void ssp_slave(bool_t enable)
{
	if(enable) {
		SSP->CR1 |= CR1_MS;
	} else {
		SSP->CR1 &= ~CR1_MS;
	}
}

void ssp_slave_output(bool_t enable)
{
	if(enable) {
		SSP->CR1 |= CR1_SOD;
	} else {
		SSP->CR1 &= ~CR1_SOD;
	}
}

void ssp_loopback(bool_t enable)
{
	if(enable) {
		SSP->CR1 |= CR1_LBM;
	} else {
		SSP->CR1 &= ~CR1_LBM;
	}
}

void ssp_clock(uint32_t f)
{
	uint32_t div = HalSysGetPCLK() / f;

	if(div < 2) {
		div = 2; // XXx
	}
	if(div > 254) {
		div = 254; // XXX
	}

	div++;
	div &= ~1;

	SSP->CPSR = div & 0xFF;
}

uint8_t ssp_transfer(uint8_t d)
{
	WAIT_TXF_NOTFULL();

	SSP->DR = d;

	WAIT_RXF_NOTEMPTY();

	return SSP->DR;
}

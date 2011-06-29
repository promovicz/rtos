
#include <core/common.h>
#include <core/clock.h>

/* COMMANDS */
enum {
	/* register access */
	NRF_CMD_READ_REG = 0x00,  /* add 5bit reg addr */
	NRF_CMD_WRITE_REG = 0x20, /* add 5bit reg addr */

	/* fifo access */
	NRF_CMD_READ_RX_PL  = 0x61,
	NRF_CMD_WRITE_TX_PL = 0xa0,

	/* fifo flush */
	NRF_CMD_FLUSH_TX = 0xe1,
	NRF_CMD_FLUSH_RX = 0xe2,

	/* fifo control */
	NRF_CMD_REUSE_TX_PL       = 0xe3,
	NRF_CMD_READ_RX_PL_WIDTH  = 0x60,
	NRF_CMD_WRITE_ACK_PL      = 0xa8, /* add 3bit pipe addr */
	NRF_CMD_WRITE_TX_PL_NOACK = 0xb0,

	/* misc */
	NRF_CMD_NOP = 0xff,
};

/* REGISTERS */
enum {
	NRF_REG_CONFIG      = 0x00,
	NRF_REG_EN_AA       = 0x01,
	NRF_REG_EN_RXADDR   = 0x02,
	NRF_REG_SETUP_AW    = 0x03,
	NRF_REG_SETUP_RETR  = 0x04,
	NRF_REG_RF_CH       = 0x05,
	NRF_REG_RF_SETUP    = 0x06,
	NRF_REG_STATUS      = 0x07,
	NRF_REG_OBSERVE_TX  = 0x08,
	NRF_REG_RPD         = 0x09,
	NRF_REG_RX_ADDR_P0  = 0x0a,
	NRF_REG_RX_ADDR_P1  = 0x0b,
	NRF_REG_RX_ADDR_P2  = 0x0c,
	NRF_REG_RX_ADDR_P3  = 0x0d,
	NRF_REG_RX_ADDR_P4  = 0x0e,
	NRF_REG_RX_ADDR_P5  = 0x0f,
	NRF_REG_TX_ADDR     = 0x10,
	NRF_REG_RX_PW_P0    = 0x11,
	NRF_REG_RX_PW_P1    = 0x12,
	NRF_REG_RX_PW_P2    = 0x13,
	NRF_REG_RX_PW_P3    = 0x14,
	NRF_REG_RX_PW_P4    = 0x15,
	NRF_REG_RX_PW_P5    = 0x16,
	NRF_REG_FIFO_STATUS = 0x17,
	NRF_REG_DYNPD       = 0x1c,
	NRF_REG_FEATURE     = 0x1d,
};

/* CONFIG register */
enum {
	NRF_CFG_MASK_RX_DR  = (1 << 6),
	NRF_CFG_MASK_TX_DS  = (1 << 5),
	NRF_CFG_MASK_MAX_RT = (1 << 4),
	NRF_CFG_EN_CRC      = (1 << 3),
	NRF_CFG_CRCO        = (1 << 2),
	NRF_CFG_PWR_UP      = (1 << 1),
	NRF_CFG_PRIM_RX     = (1 << 0),
};

/* EN_AA register */
enum {
	NRF_EN_AA_P0 = (1 << 0),
	NRF_EN_AA_P1 = (1 << 1),
	NRF_EN_AA_P2 = (1 << 2),
	NRF_EN_AA_P3 = (1 << 3),
	NRF_EN_AA_P4 = (1 << 4),
	NRF_EN_AA_P5 = (1 << 5),
};

/* EN_RXADDR register */
enum {
	NRF_EN_RXADDR_P0 = (1 << 0),
	NRF_EN_RXADDR_P1 = (1 << 1),
	NRF_EN_RXADDR_P2 = (1 << 2),
	NRF_EN_RXADDR_P3 = (1 << 3),
	NRF_EN_RXADDR_P4 = (1 << 4),
	NRF_EN_RXADDR_P5 = (1 << 5),
};

/* SETUP_AW register */
enum {
	NRF_SETUP_AW_3 = 0x01,
	NRF_SETUP_AW_4 = 0x02,
	NRF_SETUP_AW_5 = 0x03,
};

/* SETUP_RETR register */

/* RF_CH register */

/* RF_SETUP register */
enum {
	NRF_RF_SETUP_CONT_WAVE  = (1 << 7),
	NRF_RF_SETUP_RF_DR_LOW  = (1 << 5),
	NRF_RF_SETUP_PLL_LOCK   = (1 << 4),
	NRF_RF_SETUP_RF_DR_HIGH = (1 << 3),
	NRF_RF_SETUP_RF_PWR_m18 = 0x00,
	NRF_RF_SETUP_RF_PWR_m12 = 0x02,
	NRF_RF_SETUP_RF_PWR_m6  = 0x04,
	NRF_RF_SETUP_RF_PWR_m0  = 0x06,
};

/* STATUS register */
enum {
	NRF_STATUS_RX_DR = (1 << 6),
	NRF_STATUS_TX_DS = (1 << 5),
	NRF_STATUS_MAX_RT = (1 << 4),
	/* NRF_STATUS_RX_P_NO, */
	NRF_STATUS_TX_FULL = (1 << 0),
};

/* OBSERVE_TX register */
enum {
	NRF_OBSERVE_TX_PLOS_CNT,
	NRF_OBSERVE_TX_ARC_CNT,
};

/* FEATURE register */
enum {
	NRF_FEATURE_EN_DPL     = (1 << 2),
	NRF_FEATURE_EN_ACK_PAY = (1 << 1),
	NRF_FEATURE_EN_DYN_ACK = (1 << 0),
};

struct nrf {
};

struct nrf nrf;

void nrf_init(void)
{
	/* initialize CS pin */
	gpio_pin_high(nrf.gpio_cs);
	gpio_pin_set_direction(nrf.gpio_cs, BOOL_TRUE);

	/* initialize CE pin */
	gpio_pin_low(nrf.gpio_ce);
	gpio_pin_set_direction(nrf.gpio_ce, BOOL_TRUE);

	/* initialize IRQ pin */
	gpio_pin_set_direction(nrf.gpio_irq, BOOL_FALSE);

	/* startup delay */
	clock_delay(100*NANOSEC_MSEC);
}

void nrf_cs(bool_t assert)
{
	gpio_pin_set(nrf.gpio_cs, ~assert);
}

void nrf_ce(bool_t assert)
{
	gpio_pin_set(nrf.gpio_ce, assert);
}


void nrf_write8(uint8_t addr, uint8_t data)
{
	uint8_t cmd = NRF_CMD_READ_REG + (addr & 0x1F);

	nrf_cs(BOOL_TRUE);
	ssp_transfer(cmd);
	ssp_transfer(data);
	nrf_cs(BOOL_FALSE);
}

uint8_t nrf_read8(uint8_t addr)
{
	uint8_t cmd = NRF_CMD_READ_REG + (addr & 0x1F);
	uint8_t res;

	nrf_cs(BOOL_TRUE);
	ssp_transfer(cmd);
	res = ssp_transfer(0);
	nrf_cs(BOOL_FALSE);

	return res;
}

void nrf_write(uint8_t addr, uint8_t *buf, size_t count)
{
	uint8_t cmd = NRF_CMD_READ_REG + (addr & 0x1F);
	int i;

	nrf_cs(BOOL_TRUE);
	ssp_transfer(cmd);
	for(i = 0; i < count; i++) {
		ssp_transfer(buf[i]);
	}
	nrf_cs(BOOL_FALSE);
}

void nrf_read(uint8_t addr, uint8_t *buf, size_t count)
{
	uint8_t cmd = NRF_CMD_READ_REG + (addr & 0x1F);
	int i;

	nrf_cs(BOOL_TRUE);
	ssp_transfer(cmd);
	for(i = 0; i < count; i++) {
		buf[i] = ssp_transfer(0);
	}
	nrf_cs(BOOL_FALSE);
}

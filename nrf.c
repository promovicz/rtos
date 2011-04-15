
enum {
	/* register access *(
	NRF_CMD_READ_REG,
	NRF_CMD_WRITE_REG,

	/* fifo access */
	NRF_CMD_READ_RX_PL,
	NRF_CMD_WRITE_TX_PL,

	/* fifo flush */
	NRF_CMD_FLUSH_TX,
	NRF_CMD_FLUSH_RX,

	/* */
	NRF_CMD_REUSE_TX_PL,
	NRF_CMD_READ_RX_PL_WIDTH,
	NRF_CMD_WRITE_ACK_PL,
	NRF_CMD_WRITE_TX_PL_NOACK,

	NRF_CMD_NOP,
};

enum {
	NRF_REG_CONFIG,
	NRF_REG_EN_AA,
	NRF_REG_EN_RXADDR,
	NRF_REG_SETUP_AW,
	NRF_REG_SETUP_RETR,
	NRF_REG_RF_CH,
	NRF_REG_RF_SETUP,
	NRF_REG_STATUS,
	NRF_REG_OBSERVE_TX,
	NRF_REG_RPD,
	NRF_REG_RX_ADDR_P0,
	NRF_REG_RX_ADDR_P1,
	NRF_REG_RX_ADDR_P2,
	NRF_REG_RX_ADDR_P3,
	NRF_REG_RX_ADDR_P4,
	NRF_REG_RX_ADDR_P5,
	NRF_REG_TX_ADDR,
	NRF_REG_RX_PW_P0,
	NRF_REG_RX_PW_P1,
	NRF_REG_RX_PW_P2,
	NRF_REG_RX_PW_P3,
	NRF_REG_RX_PW_P4,
	NRF_REG_RX_PW_P5,
	NRF_REG_FIFO_STATUS,
	NRF_REG_
};

enum {
	NRF_CFG_MASK_RX_DR,
	NRF_CFG_MASK_TX_DS,
	NRF_CFG_MASK_MAX_RT,
	NRF_CFG_EN_CRC,
	NRF_CFG_CRCO,
	NRF_CFG_PWR_UP,
	NRF_CFG_PRIM_RX,
};

enum {
	NRF_EN_AA_P0,
	NRF_EN_AA_P1,
	NRF_EN_AA_P2,
	NRF_EN_AA_P3,
	NRF_EN_AA_P4,
	NRF_EN_AA_P5,
};

enum {
	NRF_EN_RXADDR_P0,
	NRF_EN_RXADDR_P1,
	NRF_EN_RXADDR_P2,
	NRF_EN_RXADDR_P3,
	NRF_EN_RXADDR_P4,
	NRF_EN_RXADDR_P5,
};

enum {
	NRF_RF_SETUP_CONT_WAVE,
	NRF_RF_SETUP_PLL_LOCK,
	NRF_RF_SETUP_RF_DR_LOW,
	NRF_RF_SETUP_RF_DR_HIGH,
	NRF_RF_SETUP_RF_PWR,
};

enum {
	NRF_STATUS_RX_DR,
	NRF_STATUS_TX_DS,
	NRF_STATUS_MAX_RT,
	NRF_STATUS_RX_P_NO,
	NRF_STATUS_TX_FULL,
};

enum {
	NRF_OBSERVE_TX_PLOS_CNT,
	NRF_OBSERVE_TX_ARC_CNT,
};

struct nrf {
};

struct nrf nrf;

void nrf_init(void)
{
}
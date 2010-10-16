
#include <lpc/ssp.h>

#include <stdio.h>

extern void csel_scp(bool_t yeah);

enum scp_regs {
	REVID = 0x00,
	DATAWR = 0x01,
	ADDPTR = 0x02,
	OPERATION = 0x03,
	OPSTATUS = 0x04,
	RSTR = 0x06,
	STATUS = 0x07,
	DATARD8 = 0x1f,
	DATARD16 = 0x20,
	TEMPOUT = 0x21,
};

enum opstatus_bits {
	OPSTATUS_RUNNING = (1<<0),
};

enum status_bits {
	STATUS_STARTING = (1<<0),
	STATUS_RTERROR = (1<<4),
	STATUS_DATAREADY = (1<<5),
	STATUS_EXTTRIGGED = (1<<6),
};

enum operation_codes {
	OPERATION_IDLE = 0x00,
	OPERATION_READ_INDIRECT = 0x01,
	OPERATION_WRITE_INDIRECT = 0x02,
	OPERATION_READ_EEPROM = 0x05,
	OPERATION_WRITE_EEPROM = 0x06,
	OPERATION_INIT = 0x07,
	OPERATION_MEASURE_HIGHSPEED = 0x09,
	OPERATION_MEASURE_HIGHRES = 0x0A,
	OPERATION_MEASURE_ULTRALOWPOWER = 0x0B,
	OPERATION_MEASURE_LOWPOWER = 0x0C,
	OPERATION_SELFTEST = 0x0F,
};

uint8_t scp_read_r8(uint8_t addr)
{
	uint8_t r;

	csel_scp(1);

	r = ssp_transfer(addr<<2);
	r = ssp_transfer(0x00);

	csel_scp(0);

	return r;
}

uint16_t scp_read_r16(uint8_t addr)
{
	uint16_t res;
	uint8_t r0, r1, r2;

	csel_scp(1);

	r0 = ssp_transfer(addr<<2);
	r1 = ssp_transfer(0x00);
	r2 = ssp_transfer(0x00);

	res = (r1 << 8) | r2;

	csel_scp(0);

	return res;
}

void scp_write_r8(uint8_t addr, uint8_t value)
{
	uint8_t r;

	csel_scp(1);

	r = ssp_transfer((addr<<2)|2);
	r = ssp_transfer(value);

	csel_scp(0);

	return;
}

void scp_reset(void)
{
	scp_write_r8(RSTR, 0x01);
}

extern void tick_delay(uint32_t d);

void scp_init(void)
{
	uint8_t r;

	r = scp_read_r8(STATUS);
	if(!(r&STATUS_STARTING)) {
		printf("scp reset\n");
		scp_reset();
	}

	tick_delay(50);

	int i = 0;
	do {
		tick_delay(10);
		r = scp_read_r8(STATUS);
		if(i++ > 10) {
			printf("scp init failed\n");
		}
	} while(r&STATUS_STARTING);

	r = scp_read_r8(DATARD8);
	if(!(r&1)) {
		printf("scp eeprom checksum error\n");
	}

	printf("scp revision %d\n", scp_read_r8(REVID));
}

void scp_waitidle(void)
{
	uint8_t r;
	do {
		tick_delay(10);
		r = scp_read_r8(OPERATION);
	} while(r != OPERATION_IDLE);
}

void scp_waitdata(void)
{
	uint8_t r;
	do {
		tick_delay(10);
		r = scp_read_r8(STATUS);
	} while (!(r&STATUS_DATAREADY));
}

void scp_selftest(void)
{
	uint8_t r;

	printf("commencing scp selftest...");

	scp_write_r8(OPERATION, OPERATION_SELFTEST);

	scp_waitidle();

	r = scp_read_r8(DATARD8);
	if(r == 1) {
		printf("succeeded\n");
	} else {
		printf("failed!\n");
	}
}

void scp_measure(void)
{
	uint32_t pres = 0;
	uint16_t temp = 0;

	printf("commencing scp measurement...");

	scp_write_r8(OPERATION, OPERATION_MEASURE_LOWPOWER);

	scp_waitdata();

	printf("done\n");

	temp = scp_read_r16(TEMPOUT);

	pres = scp_read_r8(DATARD8) << 16;
	pres |= scp_read_r16(DATARD16);

	printf("scp pressure %d.%02d pascals\n", pres/4, (pres%4)*25);
	printf("scp temperature %d.%02d degrees celsius\n", temp/20, (temp%20)*5);

}

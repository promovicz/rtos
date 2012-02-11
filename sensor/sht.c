
#include <platform/lpc21/gpio.h>
#include <platform/lpc21/wdt.h>

#include <stdio.h>
#include <unistd.h>

#define SHT_CMD_HUMD (0x03)
#define SHT_CMD_TEMP (0x05)

#define SHT_SDA (30)
#define SHT_SCL (29)

static void wait(void)
{
	fflush(stdout);
	wdt_kick();
	usleep(1000);
}

static void sda_input(void)
{
	gpio_pin_set_direction(0, SHT_SDA, BOOL_FALSE);
	wait();
}

static void sda_output(void)
{
	gpio_pin_set_direction(0, SHT_SDA, BOOL_FALSE);
	wait();
}

static void sda_set(bool_t v)
{
	gpio_pin_set(0, SHT_SDA, v);
	wait();
}

static bool_t sda_get(void)
{
	bool_t r = gpio_pin_get(0, SHT_SDA);
	wait();
	return r;
}

static void scl_low(void)
{
	gpio_pin_low(0, SHT_SCL);
	wait();
}

static void scl_high(void)
{
	gpio_pin_high(0, SHT_SCL);
	wait();
}

static void sht_sendb(uint8_t cmd)
{
	uint8_t i;

	sda_output();

	for(i = 0; i < 8; i++) {
		scl_low();
		sda_set((cmd>>i)&1);
		scl_high();
	}

	scl_low();
	sda_input();
	printf("foo!\n");
	while(sda_get());
	scl_high();
	scl_low();

	printf("bar!\n");
	while(!sda_get());

	printf("bla!\n");
	i = 0;
	while(sda_get()) {
		i++;
		if(i == 255) break;

		usleep(10*1000);
	}
}

static uint16_t sht_readw(void)
{
	uint8_t i;
	uint16_t r = 0;

	sda_input();

	for(i = 0; i < 8; i++) {
		scl_high();
		r |= sda_get()?(1<<i):0;
		scl_low();
	}

	sda_output();
	sda_set(0);
	scl_high();
	scl_low();
	sda_input();

	for(i = 8; i < 16; i++) {
		scl_high();
		r |= sda_get()?(1<<i):0;
		scl_low();
	}

	return r;
}

void sht_start(void)
{
	sda_output();

	sda_set(0);
	scl_low();
	scl_high();
	sda_set(1);
	scl_low();
}

void sht_measure(void)
{
	uint16_t res;

	printf("sht measure humd\n");
	printf(" start\n");
	sht_start();
	printf(" command\n");
	sht_sendb(SHT_CMD_HUMD);
	printf(" data\n");
	res = sht_readw();
	printf(" result: %d\n", res);

	printf("sht measure temp\n");
	printf(" start\n");
	sht_start();
	printf(" command\n");
	sht_sendb(SHT_CMD_TEMP);
	printf(" data\n");
	res = sht_readw();
	printf(" result: %d\n", res);
}

void sht_init(void)
{
	printf("sht init\n");

	gpio_pin_high(0, SHT_SDA);
	gpio_pin_set_direction(0, SHT_SDA, BOOL_TRUE);

	gpio_pin_high(0, SHT_SCL);
	gpio_pin_set_direction(0, SHT_SCL, BOOL_TRUE);

}


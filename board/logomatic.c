
#include "logomatic.h"

#include <lpc/pll.h>
#include <lpc/mam.h>
#include <lpc/vpb.h>
#include <lpc/pin.h>
#include <lpc/spi.h>
#include <lpc/eint.h>
#include <lpc/gpio.h>

#define GPIO_LED_STAT1 11
#define GPIO_LED_STAT2 2

static void led_init(void)
{
	gpio_pin_high(0, GPIO_LED_STAT1);
	gpio_pin_high(0, GPIO_LED_STAT2);

	gpio_pin_set_direction(0, GPIO_LED_STAT1, 1);
	gpio_pin_set_direction(0, GPIO_LED_STAT2, 1);
}

void led_stat1(bool_t lit)
{
	if(lit) {
		gpio_pin_low(0, GPIO_LED_STAT1);
	} else {
		gpio_pin_high(0, GPIO_LED_STAT1);

	}
}

void led_stat2(bool_t lit)
{
	if(lit) {
		gpio_pin_low(0, GPIO_LED_STAT2);
	} else {
		gpio_pin_high(0, GPIO_LED_STAT2);
	}
}

void board_init(void)
{
	// enable status leds early for debugging
	led_init();

	// boot pll and set full throttle on various things
	pll_configure(PLL_CORE, &pll_60mhz);
	pll_lock_blocking(PLL_CORE);
	pll_connect(PLL_CORE);

	mam_init();

	vpb_init();

	// IMU
	pin_set_function(PIN0, PIN_FUNCTION_UART0_TXD);
	pin_set_function(PIN1, PIN_FUNCTION_UART0_RXD);

	// STOP button
	pin_set_function(PIN3, PIN_FUNCTION_EINT1);
	eint_configure(EINT1, 1, 0);

	// MMC
	pin_set_function(PIN4, PIN_FUNCTION_SPI_SCK);
	pin_set_function(PIN5, PIN_FUNCTION_SPI_MISO);
	pin_set_function(PIN6, PIN_FUNCTION_SPI_MOSI);

	// GPS serial
	pin_set_function(PIN8, PIN_FUNCTION_UART1_TXD);
	pin_set_function(PIN9, PIN_FUNCTION_UART1_RXD);

	// SCP pressure sensor
	pin_set_function(PIN17, PIN_FUNCTION_SSP_SCK);
	pin_set_function(PIN18, PIN_FUNCTION_SSP_MISO);
	pin_set_function(PIN19, PIN_FUNCTION_SSP_MOSI);
}


#include "logomatic.h"

#include <core/tick.h>
#include <core/timer.h>

#include <lpc/vic.h>
#include <lpc/pll.h>
#include <lpc/mam.h>
#include <lpc/vpb.h>
#include <lpc/pin.h>
#include <lpc/spi.h>
#include <lpc/rtc.h>
#include <lpc/eint.h>
#include <lpc/gpio.h>
#include <lpc/uart.h>
#include <lpc/timer.h>
#include <lpc/pinsel.h>

#define GPIO_LED_STAT1 11
#define GPIO_LED_STAT2 2

#define INTV_STOP     0
#define	INTV_USB	  1
#define INTV_UART0    2
#define INTV_UART1    3
#define INTV_TIMER0   4
#define INTV_TIMER1   5

DEFINE_EINT_VECTOR(eint1_vector, EINT1);
DEFINE_UART_VECTOR(uart0_vector, 0);
DEFINE_UART_VECTOR(uart1_vector, 1);
DEFINE_TIMER_VECTOR(timer0_vector, 0);
DEFINE_TIMER_VECTOR(timer1_vector, 1);


static void timer_tick (always_unused int t, always_unused timer_match_t m)
{
	tick_handler();
}


static void led_init(void)
{
	gpio_pin_high(GPIO_LED_STAT1);
	gpio_pin_high(GPIO_LED_STAT2);

	gpio_pin_set_direction(GPIO_LED_STAT1, 1);
	gpio_pin_set_direction(GPIO_LED_STAT2, 1);
}

void led_stat1(bool_t lit)
{
	if(lit) {
		gpio_pin_low(GPIO_LED_STAT1);
	} else {
		gpio_pin_high(GPIO_LED_STAT1);
	}
}

void led_stat2(bool_t lit)
{
	if(lit) {
		gpio_pin_low(GPIO_LED_STAT2);
	} else {
		gpio_pin_high(GPIO_LED_STAT2);
	}
}


stop_handler_t the_stop_handler;

void set_stop_handler(stop_handler_t handler)
{
	the_stop_handler = handler;
}

static void stop_handler(int eint)
{
	led_stat2(1);

	if(the_stop_handler) {
		the_stop_handler();
	}
}


void heartbeat_function (always_unused struct timer *t, tick_t now)
{
	tick_t second = now / 1000;
	bool_t odd = second & 1 ? BOOL_TRUE : BOOL_FALSE;

	led_stat1(odd);
}

DEFINE_PERIODIC_TIMER(heartbeat, heartbeat_function, 1 * TICK_SECOND);


static nanosecs_t clktimer_read (struct clock_device *clock)
{
	return timer_read(TIMER1) * NANOSECS_USEC;
}

struct clock_device clktimer = {
	.dev = { .name = "clktimer",
			 .class = DEVICE_CLASS_CLOCK },
	.resolution = 1 * NANOSECS_USEC,
	.read = &clktimer_read,
};


void board_early_init(void)
{
	// enable status leds early for debugging
	led_init();

	// boot pll and set full throttle on various things
	pll_configure(PLL_CORE, &pll_60mhz);
	pll_lock_blocking(PLL_CORE);
	pll_connect(PLL_CORE);

	// initialize additional busses and memories
	mam_init();
	vpb_init();

	// initialize the real time clock
	rtc_init();

	// STOP button
	pin_set_function(PIN3, PIN_FUNCTION_EINT1);
	eint_configure(EINT1, 1, 0);
	eint_handler(EINT1, &stop_handler);
	vic_configure(INTV_STOP, INT_EINT1, &eint1_vector);
	vic_enable(INT_EINT1);

	// TIMER0 at 1 MHz, MR0 at 1000 Hz
	timer_init(TIMER0);
	timer_prescale(TIMER0, 60);
	timer_match_configure(TIMER0, MR0, 1000, TIMER_MATCH_INTERRUPT|TIMER_MATCH_RESET);
	timer_match_handler(TIMER0, MR0, &timer_tick);
	timer_enable(TIMER0, BOOL_TRUE);
	vic_configure(INTV_TIMER0, INT_TIMER0, &timer0_vector);
	vic_enable(INT_TIMER0);

	// TIMER1
	timer_init(TIMER1);
	timer_prescale(TIMER1, 60);
	timer_enable(TIMER1, BOOL_TRUE);
	device_add(&clktimer.dev);

	// UART0
	pin_set_function(PIN0, PIN_FUNCTION_UART0_TXD);
	pin_set_function(PIN1, PIN_FUNCTION_UART0_RXD);
	uart_init(0, 38400);
	vic_configure(INTV_UART0, INT_UART0, &uart0_vector);
	vic_enable(INT_UART0);

	// UART1
	pin_set_function(PIN8, PIN_FUNCTION_UART1_TXD);
	pin_set_function(PIN9, PIN_FUNCTION_UART1_RXD);
	uart_init(1, 9600);
	vic_configure(INTV_UART1, INT_UART1, &uart1_vector);
	vic_enable(INT_UART1);
}

void board_init(void)
{
	START_PERIODIC_TIMER(heartbeat);
}

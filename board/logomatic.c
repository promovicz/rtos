
#include "logomatic.h"

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


void heartbeat_function (always_unused struct timer *t, nanosecs_t now)
{
	nanosecs_t second = now / NANOSECS_SEC;
	bool_t odd = second & 1 ? BOOL_TRUE : BOOL_FALSE;

	led_stat1(odd);
}

DEFINE_PERIODIC_TIMER(heartbeat, heartbeat_function, 0.5 * NANOSECS_SEC);


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



#define LPC_TIMER TIMER0
#define LPC_TIMER_MR MR0

static int lpc_timer_initialize (struct timer_device *dev);
static int lpc_timer_program (struct timer_device *dev, int mode, nanosecs_t delta);

struct timer_device lpc_timer = {
	.dev = { .name = "systimer",
			 .class = DEVICE_CLASS_TIMER },
	.features = TIMER_FEAT_ONESHOT | TIMER_FEAT_PERIODIC,
	.min_delta = 10 * NANOSECS_USEC,
	.max_delta = 10 * NANOSECS_SEC,
	.initialize = &lpc_timer_initialize,
	.program = &lpc_timer_program,
};

static void lpc_timer_match (always_unused int t, always_unused timer_match_t m) {
	if(lpc_timer.callback) {
		lpc_timer.callback(&lpc_timer);
	}
}

static int lpc_timer_initialize (struct timer_device *dev) {
	timer_init(LPC_TIMER);
	timer_prescale(LPC_TIMER, 60);
	timer_reset(LPC_TIMER);
	timer_match_handler(LPC_TIMER, LPC_TIMER_MR, &lpc_timer_match);
	timer_match_configure(LPC_TIMER, LPC_TIMER_MR, 1000, 0);

	dev->program(dev, TIMER_MODE_DISABLED, 0);

	return 0;
}

static int lpc_timer_program (struct timer_device *dev, int mode, nanosecs_t delta) {
	uint32_t delta_cycles = delta / NANOSECS_USEC;
	uint32_t match_cycles;

	/* always stop first */
	timer_enable(LPC_TIMER, BOOL_FALSE);

	match_cycles = timer_read(LPC_TIMER) + delta_cycles;

	/* adjust timer config */
	switch(mode) {
	case TIMER_MODE_DISABLED:
		timer_match_configure(LPC_TIMER, LPC_TIMER_MR, 1000, 0);
		break;
	case TIMER_MODE_ONESHOT:
		timer_match_configure(LPC_TIMER, LPC_TIMER_MR,
							  match_cycles,
							  TIMER_MATCH_INTERRUPT|TIMER_MATCH_STOP);
		timer_enable(LPC_TIMER, BOOL_TRUE);
		break;
	case TIMER_MODE_PERIODIC:
		timer_match_configure(LPC_TIMER, LPC_TIMER_MR,
							  match_cycles,
							  TIMER_MATCH_INTERRUPT|TIMER_MATCH_RESET);
		timer_enable(LPC_TIMER, BOOL_TRUE);
		break;
	default:
		return -1;
	}

	/* all good */
	return 0;
}



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

	// TIMER0 as system timer
	device_add(&lpc_timer.dev);
	vic_configure(INTV_TIMER0, INT_TIMER0, &timer0_vector);
	vic_enable(INT_TIMER0);

	// TIMER1 as clock timer
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

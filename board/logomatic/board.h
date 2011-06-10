#ifndef BOARD_LOGOMATIC_H
#define BOARD_LOGOMATIC_H

#include <core/types.h>

#define INTV_STOP     0
#define	INTV_USB	  1
#define INTV_UART0    2
#define INTV_UART1    3
#define INTV_TIMER0   4
#define INTV_TIMER1   5

#define LPC_CLOCK_TIMER TIMER1

#define LPC_TIMER TIMER0
#define LPC_TIMER_MR MR0

#define GPIO_LED_STAT1 11
#define GPIO_LED_STAT2 2

void board_early_init(void);
void board_init(void);

void led_stat1(bool_t lit);
void led_stat2(bool_t lit);

typedef void (*stop_handler_t) (void);
void set_stop_handler(stop_handler_t handler);

#endif /* !BOARD_LOGOMATIC_H */

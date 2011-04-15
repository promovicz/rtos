#ifndef BOARD_LOGOMATIC_H
#define BOARD_LOGOMATIC_H

#include <core/types.h>

void board_early_init(void);
void board_init(void);

void led_stat1(bool_t lit);
void led_stat2(bool_t lit);

typedef void (*stop_handler_t) (void);
void set_stop_handler(stop_handler_t handler);

#endif /* !BOARD_LOGOMATIC_H */

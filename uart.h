
#ifndef BIKE_UART_H
#define BIKE_UART_H

#include "types.h"

typedef enum {
	UART0 = 0,
	UART1 = 1,
} uart_t;

typedef uint32_t uart_baud_t;

void uart_init(uart_t uart, uart_baud_t baudrate);

void uart_irq(uart_t uart);

void uart_tx_blocking(uart_t uart, uint8_t byte);
bool_t uart_tx_nonblocking(uart_t uart, uint8_t byte);

void uart_rx_blocking(uart_t uart, uint8_t *byte);
bool_t uart_rx_nonblocking(uart_t uart, uint8_t *byte);

#endif /* !BIKE_UART_H */


#ifndef BIKE_UART_H
#define BIKE_UART_H

#include "types.h"

void uart_init(int uart);

void uart_tx_blocking(int uart, uint8_t byte);
bool_t uart_tx_nonblocking(int uart, uint8_t byte);

void uart_rx_blocking(int uart, uint8_t *byte);
bool_t uart_rx_nonblocking(int uart, uint8_t *byte);

#endif /* !BIKE_UART_H */


#ifndef BIKE_UART_H
#define BIKE_UART_H

#include <core/types.h>

typedef int uart_t;

typedef uint32_t uart_baud_t;

void uart_init(uart_t uart, uart_baud_t baudrate);

void uart_irq(uart_t uart);

int uart_tx_fifo(uart_t uart, const void *buf, size_t nbytes);
int uart_tx_blocking(uart_t uart, const void *buf, size_t nbytes);
int uart_tx_nonblocking(uart_t uart, const void *buf, size_t nbytes);

int uart_rx_fifo(uart_t uart, void *buf, size_t nbytes);
int uart_rx_blocking(uart_t uart, void *buf, size_t nbytes);
int uart_rx_nonblocking(uart_t uart, void *buf, size_t nbytes);

#endif /* !BIKE_UART_H */

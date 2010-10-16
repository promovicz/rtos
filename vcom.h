
#ifndef VCOM_H
#define VCOM_H

#include <core/types.h>

void vcom_init(void);
void vcom_connect(bool_t connect);

int vcom_rx_fifo(void *buf, size_t nbytes);
int vcom_tx_fifo(const void *buf, size_t nbytes);

#endif /* !VCOM_H */

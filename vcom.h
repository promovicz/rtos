
#ifndef VCOM_H
#define VCOM_H

#include "types.h"

void vcom_init(void);
void vcom_connect(bool_t connect);

bool_t vcom_rx_fifo(uint8_t *c);
bool_t vcom_tx_fifo(uint8_t c);

#endif /* !VCOM_H */

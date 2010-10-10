
#ifndef SSP_H
#define SSP_H

#include "types.h"

void ssp_init(void);
void ssp_clock(uint32_t f);

void ssp_enable(bool_t enable);
void ssp_loopback(bool_t enable);
void ssp_slave(bool_t enable);
void ssp_slave_output(bool_t enable);

uint8_t ssp_transfer(uint8_t d);

#endif /* !SSP_H */

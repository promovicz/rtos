
#ifndef LPC_SSP_H
#define LPC_SSP_H

#include <core/types.h>
#include <core/cli.h>

void ssp_init(void);
void ssp_clock(uint32_t f);

void ssp_enable(bool_t enable);
void ssp_loopback(bool_t enable);
void ssp_slave(bool_t enable);
void ssp_slave_output(bool_t enable);

uint8_t ssp_transfer(uint8_t d);

void ssp_command(struct cli *t, int argc, char **argv);

void ssp_enable_trace(bool_t enable);

#endif /* !LPC_SSP_H */

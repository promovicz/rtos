#ifndef LPC_SPI_H
#define LPC_SPI_H

#include <core/types.h>
#include <core/cli.h>

void spi_init(void);

uint8_t spi_transfer(uint8_t b);

void spi_command(struct cli *t, int argc, char **argv);

void spi_enable_trace(bool_t enable);

#endif /* !LPC_SPI_H */

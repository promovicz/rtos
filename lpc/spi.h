#ifndef LPC_SPI_H
#define LPC_SPI_H

#include <core/types.h>
#include <core/tty.h>

void spi_init(void);

uint8_t spi_transfer(uint8_t b);

void spi_command(struct tty *t, int argc, char **argv);

#endif /* !LPC_SPI_H */

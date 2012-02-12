#ifndef VIC_H
#define VIC_H

#include <core/types.h>

#include <platform/lpc21/mcu.h>

typedef void (*vic_handler_t) (void);

void vic_enable(int src);
void vic_disable(int src);

void vic_default(vic_handler_t handler);

void vic_configure(int vector, int src, vic_handler_t handler);
void vic_deconfigure(int vector);

void vic_ack(void);

void vic_report(void);

#endif /* !VIC_H */

#ifndef LPC21_IRQ_H
#define LPC21_IRQ_H

#include <core/common.h>

typedef unsigned irqs_t;

irqs_t irqs_enable(void);
irqs_t irqs_disable(void);
irqs_t irqs_restore(irqs_t state);

irqs_t fiq_enable(void);
irqs_t fiq_disable(void);
irqs_t fiq_restore(irqs_t old_cpsr);

#endif /* !LPC21_IRQ_H */

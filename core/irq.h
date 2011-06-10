#ifndef CORE_IRQ_H
#define CORE_IRQ_H

#include <core/common.h>

unsigned irq_disable(void);
unsigned irq_enable(void);
unsigned irq_restore(unsigned old_cpsr);

unsigned fiq_disable(void);
unsigned fiq_enable(void);
unsigned fiq_restore(unsigned old_cpsr);

#endif /* !CORE_IRQ_H */

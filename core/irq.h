#ifndef CORE_IRQ_H
#define CORE_IRQ_H

#include <core/common.h>

#include <platform/lpc21/irq.h>

typedef uint32_t irq_flags_t;

typedef int (*irq_handler_t) (int line, void *cookie);

struct irq_line {
	int index;
	irq_flags_t flags;
	irq_handler_t handler;
	void *cookie;
};

typedef void (*iop_ack)     (struct irq_line *line);
typedef void (*iop_mask)    (struct irq_line *line);
typedef void (*iop_unmask)  (struct irq_line *line);

struct irq_chip {
	const char *name;

	int base_line;
	int num_lines;

	struct irq_line *lines;

	iop_initialize_t iop_initialize;

	iop_ack_t    iop_ack;
	iop_mask_t   iop_mask;
	iop_unmask_t iop_unmask;
};

void irq_chip_register(struct irq_chip *chip);

int irq_request(int line, irq_flags_t flags,
				irq_handler_t handler, void *cookie);

int irq_release(int line);

#endif /* !CORE_IRQ_H */

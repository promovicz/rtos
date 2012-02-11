
#include <core/irq.h>

void
irq_chip_register(struct irq_chip *chip)
{
	irqs_t stat;

	stat = irqs_disable();

	irqs_restore(stat);
}

int
irq_request(int line, irq_flags_t flags,
			irq_handler_t handler, void *cookie)
{
	irqs_t stat;

	stat = irqs_disable();

	irqs_restore(stat);
}

int
irq_release(int line)
{
	irqs_t stat;

	stat = irqs_disable();

	irqs_restore(stat);
}

int
irq_handle_edge(struct irq_line *line)
{
}

int
irq_handle_level(struct irq_line *line)
{
}

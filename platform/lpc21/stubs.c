
#include <stdlib.h>
#include <stdio.h>

#include <core/irq.h>

/* interrupt handler stubs */

void stub_irq (void)   __attribute__ ((interrupt("IRQ")));
void stub_fiq (void)   __attribute__ ((interrupt("FIQ")));
void stub_swi (void)   __attribute__ ((interrupt("SWI")));
void stub_dabort (void) __attribute__ ((interrupt("ABORT")));
void stub_pabort (void) __attribute__ ((interrupt("ABORT")));
void stub_undef (void) __attribute__ ((interrupt("UNDEF")));

void stub_irq (void) {
	printf("UNEXPECTED IRQ\n");
	irqs_enable();
	abort();
}

void stub_fiq (void)  {
	printf("UNEXPECTED FIQ\n");
	irqs_enable();
	abort();
}

void stub_swi (void)  {
	printf("UNEXPECTED SWI\n");
	irqs_enable();
	abort();
}

void stub_dabort (void) {
	printf("DATA ABORT\n");
	irqs_enable();
	abort();
}

void stub_pabort (void) {
	printf("PROGRAM ABORT\n");
	irqs_enable();
	abort();
}

void stub_undef (void) {
	printf("UNDEFINED INSTRUCTION\n");
	irqs_enable();
	abort();
}

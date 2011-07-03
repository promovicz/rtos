
/* interrupt handler stubs */

void stub_irq (void)   __attribute__ ((interrupt("IRQ")));
void stub_fiq (void)   __attribute__ ((interrupt("FIQ")));
void stub_swi (void)   __attribute__ ((interrupt("SWI")));
void stub_undef (void) __attribute__ ((interrupt("UNDEF")));

void stub_irq (void) {
        while (1) ;     
}

void stub_fiq (void)  {
        while (1) ;     
}

void stub_swi (void)  {
        while (1) ;     
}

void stub_undef (void) {
        while (1) ;     
}

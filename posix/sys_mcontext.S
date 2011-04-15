
.section .text

/* void _mcontext_swap (void **save_sp [r0], void *new_sp [r1]) */
.global _mcontext_swap
.func _mcontext_swap
		/* save registers onto current stack */
		stmfd sp!, {r4 - r11, lr}
		/* save current stack pointer (old context) */
		str   sp, [r0]
		/* load new stack pointer */
		ldr	  sp, [r1]
		/* load registers from new stack */
		ldmfd sp!, {r4 - r11, pc}
.endfunc


.text

.global _mcontext_entry
.func 	_mcontext_entry
_mcontext_entry:
		/* r4 contains uc_link  */
		/* r5 contains function */
		ldr lr, _mcontext_exit
		mov pc, r5
_mcontext_exit:	
		mov r0, r4
		bl setcontext
		mov r0, #0
		bl exit
.endfunc
		
/* void _mcontext_get (void **save_sp [r0]) */
.global _mcontext_get
.func	_mcontext_get
_mcontext_get:	
		mov r1, sp
		ldr sp, [r0]
		stmfd sp!, {r4 - r11, lr}
		str sp, [r0]
		mov sp, r1
		mov pc, lr
.endfunc

/* void _mcontext_set (void **saved_sp [r0]) */
.global _mcontext_set
.func   _mcontext_set
_mcontext_set:	
		ldr sp, [r0]
		ldmfd sp!, {r4 - r11, pc}
.endfunc
		
/* void _mcontext_swap (void **save_sp [r0], void *new_sp [r1]) */
.global _mcontext_swap
.func _mcontext_swap
_mcontext_swap:	
		/* save registers onto current stack */
		stmfd sp!, {r4 - r11, lr}
		/* save current stack pointer (old context) */
		str   sp, [r0]
		/* load new stack pointer */
		ldr	  sp, [r1]
		/* load registers from new stack */
		ldmfd sp!, {r4 - r11, pc}
.endfunc

.end

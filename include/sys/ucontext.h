
#ifndef UCONTEXT_H
#define UCONTEXT_H

#include <signal.h>

typedef struct mstack {
	uint32_t ms_r4;
	uint32_t ms_r5;
	uint32_t ms_r6;
	uint32_t ms_r7;
	uint32_t ms_r8;
	uint32_t ms_r9;
	uint32_t ms_r10;
	uint32_t ms_r11;
	uint32_t ms_pc;
} mstack_t;

typedef struct mcontext {
	uint32_t *mc_sp;
} mcontext_t;

typedef struct ucontext {
	struct ucontext *uc_link;
	sigset_t uc_sigmask;
	stack_t uc_stack;
	mcontext_t uc_mcontext;
} ucontext_t;

void makecontext(ucontext_t *ucp, void (*func)(), int argc, ...);
int swapcontext(ucontext_t *oucp, ucontext_t *ucp);
int getcontext(ucontext_t *ucp);
int setcontext(const ucontext_t *ucp);

#endif

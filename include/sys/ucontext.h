
#ifndef UCONTEXT_H
#define UCONTEXT_H

#include <signal.h>

typedef struct mcontext {
	void *mc_sp;
} mcontext_t;

typedef struct ucontext {
	struct ucontext *uc_link;
	sigset_t uc_sigmask;
	stack_t uc_stack;
	mcontext_t uc_mcontext;
} ucontext_t;

#endif


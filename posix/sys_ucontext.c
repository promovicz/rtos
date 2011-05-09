
#include <stdio.h>

#include <sys/ucontext.h>

extern void _mcontext_entry(void);

extern void _mcontext_get(void *sp);
extern void _mcontext_set(const void *sp);
extern void _mcontext_swap(void *osp, void *sp);

int getcontext(ucontext_t *ucp) {
	_mcontext_get(&ucp->uc_mcontext.mc_sp);
	return 0;
}

int setcontext(const ucontext_t *ucp) {
	_mcontext_set(&ucp->uc_mcontext.mc_sp);
	return 0;
}

void makecontext(ucontext_t *ucp, void (*func)(), int argc, ...) {
	mcontext_t *mcp = &ucp->uc_mcontext;
	stack_t *sp = &ucp->uc_stack;
	mstack_t *state;
	unsigned char *isp;

	/* XXX: does not handle cast alignment issues */
	isp = ((unsigned char *)sp->ss_sp)
		+ sp->ss_size
		- sizeof(mstack_t);

	mcp->mc_sp = (uint32_t *)isp;

	state = (mstack_t*)mcp->mc_sp;

	memset(state, 0, sizeof(*state));

	state->ms_r4 = (uint32_t)ucp->uc_link;
	state->ms_pc = (uint32_t)func;
	//state->ms_pc = &_mcontext_entry;

	printf("st is %p\n", (void*)state);
	printf("pc is %p\n", (void*)state->ms_pc);
}


int swapcontext(ucontext_t *oucp, ucontext_t *ucp) {

	/* switch the context */
	_mcontext_swap(&oucp->uc_mcontext.mc_sp,
				  &ucp->uc_mcontext.mc_sp);


	return 0;
}


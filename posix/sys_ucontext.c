
#include <sys/ucontext.h>

int getcontext(ucontext_t *ucp) {
	return 0;
}

int setcontext(ucontext_t *ucp) {
	return 0;
}

void makecontext(ucontext_t *ucp, void (*func)(), int argc, ...) {
	
}


void _mcontext_swap(void **osp, void **sp);

int swapcontext(ucontext_t *oucp, ucontext_t *ucp) {

	/* switch the context */
	_mcontext_swap(&oucp->uc_mcontext.mc_sp,
				  &ucp->uc_mcontext.mc_sp);

	return 0;
}



#include <sys/types.h>

#include <signal.h>

int __rt_sigprocmask(int how, const sigset_t *set, sigset_t *oldsetm, long nr)
{
	return 0;
}

#if 0
int rt_sigprocmask(int how, const sigset_t *set, sigset_t *oldsetm, long nr)
	weak_alias("__rt_sigprocmask");
#endif

int kill(pid_t pid, int sig)
{
	return 0;
}


#include <sys/mman.h>

int mlock(const void *addr, size_t len)
{
	return 0;
}

int munlock(const void *addr, size_t len)
{
	return 0;
}

int mlockall(int flags)
{
	return 0;
}

int munlockall(void)
{
	return 0;
}

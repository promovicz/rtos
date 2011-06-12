
#include "memory.h"

#include <unistd.h>

size_t __libc_getpagesize(void)
{
	return PAGESIZE;
}

size_t __getpagesize(void)
	weak_alias("__libc_getpagesize");

size_t getpagesize(void)
	weak_alias("__libc_getpagesize");

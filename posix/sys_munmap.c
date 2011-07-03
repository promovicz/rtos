
#include "common.h"

#include <core/memory.h>

#include <errno.h>
#include <stdio.h>

#include <sys/mman.h>

int munmap(void *addr, size_t length)
{
	int i, count, start;

	if(((uint32_t)addr) % PAGESIZE) {
		errno = EINVAL;
		return -1;
	}

	if(((char*)addr) < membase || ((char*)addr) >= (membase + pagecount * PAGESIZE)) {
		errno = EFAULT;
		return -1;
	}

	if(length % PAGESIZE) {
		errno = EINVAL;
		return -1;
	}

	count = length / PAGESIZE;

	memory_free_pages(addr, count, "posix");

	return 0;
}


#include "memory.h"

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
	start = ((uint32_t)addr - (uint32_t)membase) / PAGESIZE;

	printf("unmapping %d pages at %p\n", count, membase + start * PAGESIZE);

	for(i = 0; i < count; i++) {
		pagetable[start + i].flags &= ~PAGE_FLAG_ALLOC;
	}

	return 0;
}

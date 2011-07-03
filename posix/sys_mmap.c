
#include "common.h"

#include <core/memory.h>

#include <errno.h>
#include <stdio.h>

#include <sys/mman.h>

void *mmap(void *addr, size_t length,
		   int prot, int flags,
		   int fd, off_t offset)
{
	void *res;
	size_t sizepgs;

	if(!(flags & MAP_ANONYMOUS)) {
		errno = EINVAL;
		return NULL;
	}

	if(flags & MAP_FIXED) {
		errno = EINVAL;
		return NULL;
	}

	if(flags & MAP_GROWSDOWN) {
		errno = EINVAL;
		return NULL;
	}

	if(length % PAGESIZE) {
		errno = EINVAL;
		return NULL;
	}

	sizepgs = length / PAGESIZE;

	res = memory_alloc_pages(sizepgs, "posix");
	if(res) {
		return res;
	}

	errno = ENOMEM;
	return MAP_FAILED;
}

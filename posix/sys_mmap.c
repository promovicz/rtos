
#include "memory.h"

#include <errno.h>
#include <stdio.h>

#include <sys/mman.h>

void *mmap(void *addr, size_t length,
		   int prot, int flags,
		   int fd, off_t offset)
{
	int i, j;
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

	printf("looking for %ld pages to map\n", sizepgs);

	for(i = 0; i < (int)pagecount; i++) {
		if(!(pagetable[i].flags & PAGE_FLAG_ALLOC)) {
			bool_t blocked = BOOL_FALSE;
			for(j = i + 1; j < (int)(i + sizepgs); j++) {
				if(pagetable[i + j].flags & PAGE_FLAG_ALLOC) {
					blocked = BOOL_TRUE;
					break;
				}
			}
			if(blocked) {
				continue;
			}

			/* found a place! */

			for(j = i; j < (int)(i + sizepgs); j++) {
				pagetable[j].flags |= PAGE_FLAG_ALLOC;
			}

			char *x = membase + (i * PAGESIZE);

			printf("mapping %ld pages at %p startpage %d\n", sizepgs, x, i);

			return x;
		}
	}

	errno = ENOMEM;
	return NULL;
}

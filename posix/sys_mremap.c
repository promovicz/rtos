
#include "memory.h"

#include <errno.h>
#include <stdio.h>

#include <sys/mman.h>

void *mremap(void *old_address,
			 size_t old_size, size_t new_size,
			 unsigned long maymove)
{
	int startpg, tailpg, tailcount, i;

	if(((uint32_t)old_address) % PAGESIZE) {
		errno = EINVAL;
		return NULL;
	}

	if(((char*)old_address) < membase || ((char*)old_address) >= (membase + pagecount * PAGESIZE)) {
		errno = EFAULT;
		return NULL;
	}

	if(old_size % PAGESIZE || new_size % PAGESIZE) {
		errno = EINVAL;
		return NULL;
	}

	if(maymove) {
		errno = EOPNOTSUPP;
		return NULL;
	}

	startpg = ((uint32_t)old_address - (uint32_t)membase) / PAGESIZE;

	if(new_size == old_size) {
		return old_address;
	}

	if(new_size < old_size) {
		printf("shrinking at %d\n", startpg);
		tailpg = startpg + new_size / PAGESIZE;
		tailcount = (old_size - new_size) / PAGESIZE;
		for(i = 0; i < tailcount; i++) {
			pagetable[tailpg+i].flags &= ~PAGE_FLAG_ALLOC;
		}
		return old_address;
	}

	if(new_size > old_size) {
		printf("growing at %d\n", startpg);
		tailpg = startpg + old_size / PAGESIZE;
		tailcount = (new_size - old_size) / PAGESIZE;
		for(i = 0; i < tailcount; i++) {
			if(pagetable[tailpg+i].flags & PAGE_FLAG_ALLOC) {
				printf("failed!\n");
				errno = ENOMEM;
				return NULL;
			}
		}
		for(i = 0; i < tailcount; i++) {
			pagetable[tailpg+i].flags |= PAGE_FLAG_ALLOC;
		}
		return old_address;
	}

	return NULL;
}

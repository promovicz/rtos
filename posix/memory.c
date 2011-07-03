
#include "memory.h"

#include "control.h"

#include <core/defines.h>
#include <core/types.h>

#include <errno.h>
#include <stdio.h>

#include <sys/mman.h>

extern void __heap_start, __heap_end;

char *membase;
size_t pagecount;
struct page *pagetable;

void posix_memory_init(void)
{
	uintptr_t start = (uintptr_t)&__heap_start;
	uintptr_t end = (uintptr_t)&__heap_end;
	size_t lbytes, lpages, ltable, ltablepages;

#if 0
	printf("mm initial start = %p\n", (void*)start);
	printf("mm initial end = %p\n", (void*)end);
#endif

	/* align start and end */
	if(start % PAGESIZE) {
		start += (PAGESIZE - start % PAGESIZE);
	}
	if(end % PAGESIZE) {
		end -= end % PAGESIZE;
	}

	/* compute size of available memory */
	lbytes = end - start;
	lpages = lbytes / PAGESIZE;

	/* compute size of page table */
	ltable = lpages * sizeof(struct page);
	if(ltable%PAGESIZE) {
		ltable += (PAGESIZE - ltable % PAGESIZE);
	}

	ltablepages = ltable / PAGESIZE;

#if 0
	printf("mm start = %p\n", (void*)start);
	printf("mm end = %p\n", (void*)end);
	printf("mm mem in bytes = %zd\n", lbytes);
	printf("mm mem in pages = %zd\n", lpages);
	printf("mm table in bytes = %zd\n", ltable);
	printf("mm table in pages = %zd\n", ltablepages);
#endif

	membase = ((char*)start) + ltablepages * PAGESIZE;
	pagecount = lpages - ltablepages; // XXX correct things to mark page tables in tables properly
	pagetable = (struct page *)start;

	memset(pagetable, 0, ltablepages * PAGESIZE);

#if 0
	printf("mm memory base is 0x%08x\n", membase);
	printf("mm table at 0x%08x with %d entries\n", pagetable, pagecount);
#endif
}

void posix_memory_report(void)
{
	size_t i, free, used;
	free = 0; used = 0;
	for(i = 0; i < pagecount; i++) {
		if(pagetable[i].flags & PAGE_FLAG_ALLOC) {
			used++;
		} else {
			free++;
		}
	}

	printf("memory avail: %zu bytes in %zu pages of %zu bytes\n",
		   pagecount * PAGESIZE, pagecount, PAGESIZE);
	printf("memory used: %zu of %zu bytes\n",
		   used * PAGESIZE, free * PAGESIZE);
}

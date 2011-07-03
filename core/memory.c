
#include "memory.h"

#include <core/defines.h>
#include <core/types.h>
#include <core/clock.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <sys/mman.h>

// #define MEM_DEBUG

extern void __heap_start, __heap_end;

char *membase;
size_t pagecount;
struct page *pagetable;

void memory_init(void)
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

void *memory_alloc_pages(size_t numpages, const char *user)
{
	int i, j;

#if MEM_DEBUG
	printf("looking for %ld pages to allocate\n", numpages);
	fflush(stdout);
	clock_delay(NANOSECS_MSEC*100);
#endif

	for(i = 0; i < (int)pagecount; i++) {
		if(!(pagetable[i].flags & PAGE_FLAG_ALLOC)) {
			bool_t blocked = BOOL_FALSE;
			for(j = i + 1; j < (int)(i + numpages); j++) {
				if(pagetable[i + j].flags & PAGE_FLAG_ALLOC) {
					blocked = BOOL_TRUE;
					break;
				}
			}
			if(blocked) {
				continue;
			}

			/* found a place! */

			for(j = i; j < (int)(i + numpages); j++) {
				pagetable[j].flags |= PAGE_FLAG_ALLOC;
				pagetable[j].user = (char*)user;
			}

			char *x = membase + (i * PAGESIZE);

#if MEM_DEBUG
			printf("allocating %ld pages at %p startpage %d\n", numpages, x, i);
			fflush(stdout);
			clock_delay(NANOSECS_MSEC*100);
#endif

			return x;
		}
	}

	return NULL;
}

void *memory_realloc_pages(void *curbase,
		size_t oldpagecount, size_t newpagecount, const char *user)
{
	int startpg, i, tailcount, tailpg;

	startpg = ((uint32_t)curbase - (uint32_t)membase) / PAGESIZE;

	if(newpagecount < oldpagecount) {
#if MEM_DEBUG
		printf("shrinking at %d\n", startpg);
		fflush(stdout);
		clock_delay(NANOSECS_MSEC*100);
#endif
		tailpg = startpg + newpagecount;
		tailcount = oldpagecount - newpagecount;
		for(i = 0; i < tailcount; i++) {
			pagetable[tailpg+i].flags &= ~PAGE_FLAG_ALLOC;
			pagetable[tailpg+i].user = NULL;
		}
		return curbase;
	}

	if(newpagecount > oldpagecount) {
#if MEM_DEBUG
		printf("growing at %d\n", startpg);
		fflush(stdout);
		clock_delay(NANOSECS_MSEC*100);
#endif
		tailpg = startpg + oldpagecount;
		tailcount = newpagecount - oldpagecount;
		for(i = 0; i < tailcount; i++) {
			if(pagetable[tailpg+i].flags & PAGE_FLAG_ALLOC) {
#if MEM_DEBUG
				printf("failed!\n");
				fflush(stdout);
				clock_delay(NANOSECS_MSEC*100);
#endif
				errno = ENOMEM;
				return NULL;
			}
		}
		for(i = 0; i < tailcount; i++) {
			pagetable[tailpg+i].flags |= PAGE_FLAG_ALLOC;
			pagetable[tailpg+i].user = (char*)user;
		}
		return curbase;
	}

	return NULL;
}

void memory_free_pages(void *base, size_t numpages, const char *user)
{
	int i, start;

	start = ((uint32_t)base - (uint32_t)membase) / PAGESIZE;

#if MEM_DEBUG
	printf("freeing %zu pages at %p\n", numpages, membase + start * PAGESIZE);
	fflush(stdout);
	clock_delay(NANOSECS_MSEC*100);
#endif

	for(i = 0; i < (int)numpages; i++) {
		pagetable[start + i].flags &= ~PAGE_FLAG_ALLOC;
		pagetable[start + i].user = NULL;
	}
}

void memory_report(void)
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

	printf("page usage:\n");
	for(i = 0; i < pagecount; i++) {
		char *pa = membase + i * PAGESIZE;
		if(pagetable[i].flags & PAGE_FLAG_ALLOC) {
			printf(" page at 0x%p is used by %s\n", pa, pagetable[i].user);
		} else {
#if 0
			printf(" page at 0x%p is free\n", pa);
#endif
		}
	}
}

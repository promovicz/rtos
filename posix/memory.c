
#include "control.h"

#include <core/defines.h>
#include <core/types.h>

#include <errno.h>
#include <stdio.h>

#include <sys/mman.h>

#define PAGESIZE 512


extern void __heap_start, __heap_end;

enum {
	PAGE_FLAG_ALLOC = (1<<0),
};

struct page {
	uint8_t flags;
};

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
	int i, free, used;
	free = 0; used = 0;
	for(i = 0; i < pagecount; i++) {
		if(pagetable[i].flags & PAGE_FLAG_ALLOC) {
			used++;
		} else {
			free++;
		}
	}

	printf("memory avail: %d bytes in %d pages of %d bytes\n",
		   pagecount*PAGESIZE, pagecount, PAGESIZE);
	printf("memory used: %d of %d bytes\n",
		   used*PAGESIZE, free*PAGESIZE);
}

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

	printf("looking for %d pages to map\n", sizepgs);

	for(i = 0; i < pagecount; i++) {
		if(!(pagetable[i].flags & PAGE_FLAG_ALLOC)) {
			bool_t blocked = BOOL_FALSE;
			for(j = i + 1; j < (i + sizepgs); j++) {
				if(pagetable[i + j].flags & PAGE_FLAG_ALLOC) {
					blocked = BOOL_TRUE;
				}
			}
			if(blocked) {
				continue;
			}

			/* found a place! */

			for(j = i; j < (i + sizepgs); j++) {
				pagetable[j].flags |= PAGE_FLAG_ALLOC;
			}

			char *x = membase + (i * PAGESIZE);

			printf("mapping %d pages at %08x startpage %d\n", sizepgs, x, i);

			return x;
		}
	}

	errno = ENOMEM;
	return NULL;
}

void *mremap(void *old_address,
			 size_t old_size, size_t new_size,
			 unsigned long maymove)
{
	int startpg, tailpg, tailcount, i;

	if(((uint32_t)old_address) % PAGESIZE) {
		errno = EINVAL;
		return NULL;
	}

	if(old_address < membase || old_address >= (membase + pagecount * PAGESIZE)) {
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
				printf("failed, would maymove %d!\n", maymove);
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

int munmap(void *addr, size_t length)
{
	int i, count, start;

	if(((uint32_t)addr) % PAGESIZE) {
		errno = EINVAL;
		return -1;
	}

	if(addr < membase || addr >= (membase + pagecount * PAGESIZE)) {
		errno = EFAULT;
		return -1;
	}

	if(length % PAGESIZE) {
		errno = EINVAL;
		return -1;
	}

	count = length / PAGESIZE;
	start = ((uint32_t)addr - (uint32_t)membase) / PAGESIZE;

	printf("unmapping %d pages at %08x\n", count, membase + start * PAGESIZE);

	for(i = 0; i < count; i++) {
		pagetable[start + i].flags &= ~PAGE_FLAG_ALLOC;
	}

	return 0;
}

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

size_t __libc_getpagesize(void)
{
	return PAGESIZE;
}

size_t __getpagesize(void)
	weak_alias("__libc_getpagesize");

size_t getpagesize(void)
	weak_alias("__libc_getpagesize");

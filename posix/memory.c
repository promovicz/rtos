
#include <errno.h>
#include <stdio.h>

#include <sys/mman.h>

#define PAGESIZE 512

extern char __static_memory_start, __static_memory_end;
extern char __dynamic_memory_start, __dynamic_memory_end;

struct page {
	uint32_t flags;
};

void memory_init(void)
{
	uintptr_t start = (uintptr_t)&__dynamic_memory_start;
	uintptr_t end = (uintptr_t)&__dynamic_memory_end;
	size_t lbytes, lpages, ltable, ltablepages;

	printf("mm initial start = %p\n", (void*)start);
	printf("mm initial end = %p\n", (void*)end);

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

	printf("mm start = %p\n", (void*)start);
	printf("mm end = %p\n", (void*)end);
	printf("mm mem in bytes = %zd\n", lbytes);
	printf("mm mem in pages = %zd\n", lpages);
	printf("mm table in bytes = %zd\n", ltable);
	printf("mm table in pages = %zd\n", ltablepages);
}

void *mmap(void *addr, size_t length,
		   int prot, int flags,
		   int fd, off_t offset)
{
	errno = EOPNOTSUPP;
	return -1;
}

void *mremap(void *old_address,
			 size_t old_size, size_t new_size,
			 unsigned long maymove)
{
	errno = EOPNOTSUPP;
	return -1;
}

int munmap(void *addr, size_t length)
{
	errno = EOPNOTSUPP;
	return -1;
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

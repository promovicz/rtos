#ifndef POSIX_MEMORY_H
#define POSIX_MEMORY_H

#include "control.h"

#define PAGESIZE 512

enum {
	PAGE_FLAG_ALLOC = (1<<0),
};

struct page {
	uint8_t flags;
};

extern char *membase;
extern size_t pagecount;
extern struct page *pagetable;

#endif /* !POSIX_MEMORY_H */

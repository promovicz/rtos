#ifndef CORE_MEMORY_H
#define CORE_MEMORY_H

#include "common.h"

#define PAGESIZE ((size_t)512)

enum {
	PAGE_FLAG_ALLOC = (1<<0),
};

struct page {
	char *user;
	uint8_t flags;
};

extern char *membase;
extern size_t pagecount;
extern struct page *pagetable;

void memory_init (void);

void *memory_alloc_pages(size_t numpages, const char *user);
void *memory_realloc_pages(void *oldbase, size_t oldnumpages, size_t newnumpages, const char *user);
void memory_free_pages(void *base, size_t numpages, const char *user);

void memory_report (void);

#endif /* !CORE_MEMORY_H */

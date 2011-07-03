
#include "common.h"

#include <core/memory.h>

#include <errno.h>
#include <stdio.h>

#include <sys/mman.h>

void *mremap(void *old_address,
			 size_t old_size, size_t new_size,
			 unsigned long maymove)
{
	size_t oldsizepgs, newsizepgs;
	void *res;

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

	if(new_size == old_size) {
		return old_address;
	}

	oldsizepgs = old_size / PAGESIZE;
	newsizepgs = new_size / PAGESIZE;

	res = memory_realloc_pages(old_address, oldsizepgs, newsizepgs, "posix");
	if(res) {
		return res;
	}

	errno = ENOMEM;
	return MAP_FAILED;
}

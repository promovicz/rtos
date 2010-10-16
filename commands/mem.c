
#include "commands.h"

static void hexdump(const uint8_t *data, unsigned int len)
{
	const uint8_t *bufptr = data;
	const uint8_t const *endptr = bufptr + len;
	int n, m, i, hexchr;

	for (n=0; n < len; n+=32, bufptr += 32) {
		hexchr = 0;
		for(m = 0; m < 32 && bufptr < endptr; m++, bufptr++) {
			if((m) && !(m%4)) {
				putchar(' ');
				hexchr++;
			}
			printf("%02x", *bufptr);
			hexchr+=2;
		}
		bufptr -= m;
		int n = 71 - hexchr;
		for(i = 0; i < n; i++) {
			putchar(' ');
		}

		putchar(' ');

		for(m = 0; m < 32 && bufptr < endptr; m++, bufptr++) {
			if(isgraph(*bufptr)) {
				putchar(*bufptr);
			} else {
				putchar('.');
			}
		}
		bufptr -= m;

		putchar('\n');
	}
}

struct mem_region {
	char *name;
	uint32_t start;
	uint32_t size;
};

struct mem_region regions[] = {
	{"flash", 0x00000000, 0x00080000},
	{"sram",  0x40000000, 0x00008000},
	{"uram",  0x7FD00000, 0x00002000},
	{"boot",  0x7FFFD000, 0x00003000},
	{"vpb",   0xE0000000, 0x10000000},
	{"ahb",   0xF0000000, 0x10000000},
	{0,0,0},
};

struct mem_section {
	char *name;
	void *start;
	void *end;
};

struct mem_section sections[] = {
#if 0
	{".text", &__text_start, &__text_end},
	{".data", &__data_start, &__data_end},
	{".bss",  &__bss_start, &__bss_end},
#endif
	{0,0,0},
};

void mem_command(struct tty *t, int argc, char **argv)
{
	void *addr;
	void *addrx;
	long int count, i;
	uint8_t *a8;
	uint16_t *a16;
	uint32_t *a32;
	if(argc) {
		if(!strcmp("dump", argv[0])) {
			if(argc > 1) {
				if(scan_ptr(argv[1], &addr)) {
					if(!(argc > 2 && scan_hex(argv[2], &count))) {
						count = 256;
					}
					hexdump(addr, count);
				}
			}
		} else if(!strcmp("copy", argv[0])) {
			if(argc > 4) {
				if(scan_ptr(argv[1], &addr)
				   && scan_ptr(argv[2], &addrx)
				   && scan_hex(argv[3], &count)) {
					printf("copying %d bytes from %08x to %08x\n", count, (uint32_t)addrx, (uint32_t)addr);
					memcpy(addr, addrx, count);
				}
			}
		} else if (!strcmp("map", argv[0])) {
			struct mem_region *mr = regions;
			printf("memory map:\n");
			while(mr->name) {
				printf(" %6s: %08x bytes at %08x - %08x\n",
					   mr->name,
					   mr->size,
					   mr->start,
					   mr->start + mr->size - 1);
				mr++;
			}
		} else if (!strcmp("s32", argv[0])) {
		} else if (!strcmp("s16", argv[0])) {
		} else if (!strcmp("s8",  argv[0])) {
		} else if (!strcmp("l32", argv[0])) {
			if(argc > 1) {
				if(scan_ptr(argv[1], &addr)) {
					if(!(argc > 2 && scan_hex(argv[2], &count))) {
						count = 1;
					}
					a32 = (uint32_t*)(((uint32_t)addr) & ~3);
					for(i = 0; i < count; i++) {
						printf("u32 %08x: %08x\n", (uint32_t)&a32[i], a32[i]);
					}
				}
			}
		} else if (!strcmp("l16", argv[0])) {
			if(argc > 1) {
				if(scan_ptr(argv[1], &addr)) {
					if(!(argc > 2 && scan_hex(argv[2], &count))) {
						count = 1;
					}
					a16 = (uint16_t*)(((uint32_t)addr) & ~1);
					for(i = 0; i < count; i++) {
						printf("u16 %08x = %04x\n", (uint32_t)&a16[i], a16[i]);
					}
				}
			}
		} else if (!strcmp("l8",  argv[0])) {
			if(argc > 1) {
				if(scan_ptr(argv[1], &addr)) {
					if(!(argc > 2 && scan_hex(argv[2], &count))) {
						count = 1;
					}
					a8 = (uint8_t*)(addr);
					for(i = 0; i < count; i++) {
						printf("u8 %08x = %02x\n", (uint32_t)&a8[i], a8[i]);
					}
				}
			}
		}
	}
}

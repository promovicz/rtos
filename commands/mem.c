
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

extern void __text_start, __text_end;
extern void __data_start, __data_end;
extern void __bss_start, __bss_end;
extern void __heap_start, __heap_end;
extern void __stacks_start, __stacks_end;

struct mem_section sections[] = {
	{".text", &__text_start, &__text_end},
	{".data", &__data_start, &__data_end},
	{".bss",  &__bss_start, &__bss_end},
	{".heap", &__heap_start, &__heap_end},
	{".stacks", &__stacks_start, &__stacks_end},
	{0,0,0},
};

int command_mem_dump(struct cli *c, int argc, char **argv)
{
	void *addr;
	long int len;
	if(argc > 0 && scan_ptr(argv[0], &addr)) {
		if(argc > 1) {
			if(!scan_hex(argv[1], &len)) {
				printf("Invalid length.\n");
				return 1;
			}
		} else {
			len = 256;
		}
		hexdump(addr, len);
		return 0;
	}

	printf("Usage: mem dump <addr> [len]\n");

	return 1;
}

int command_mem_copy(struct cli *c, int argc, char **argv)
{
	void *dst;
	void *src;
	long int len;

	if(argc == 3) {
		if(!scan_ptr(argv[0], &dst)) {
			printf("Invalid dst address.\n");
			return 1;
		}
		if(!scan_ptr(argv[1], &src)) {
			printf("Invalid src address.\n");
			return 1;
		}
		if(!scan_hex(argv[1], &len)) {
			printf("Invalid length.\n");
			return 1;
		}

		printf("copying %d bytes to %08x from %08x\n", len, (uint32_t)dst, (uint32_t)src);
		memcpy(dst, src, len);

		return 0;
	}

	printf("Usage: mem copy <dst> <src> <len>\n");
	return 1;
}

int command_mem_regions(struct cli *c, int argc, char **argv)
{
	struct mem_region *mr = regions;
	printf("physical regions:\n");
	while(mr->name) {
		printf(" %8s: %08x bytes at %08x - %08x\n",
			   mr->name,
			   mr->size,
			   mr->start,
			   mr->start + mr->size - 1);
		mr++;
	}
}

int command_mem_sections(struct cli *c, int argc, char **argv)
{
	printf("system sections:\n");
	struct mem_section *ms = sections;
	while(ms->name) {
		printf(" %8s: %08x bytes at %08x - %08x\n",
			   ms->name,
			   ms->end - ms->start,
			   ms->start,
			   ms->end);
		ms++;
	}
}

#if 0
void mem_command(struct tty *t, int argc, char **argv)
{
	void *addr;
	void *addrx;
	long int count, i;
	uint8_t *a8;
	uint16_t *a16;
	uint32_t *a32;
	if(argc) {
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
#endif

struct command cmd_mem[] = {
	{"dump",     "dump memory at given address",    &command_mem_dump},
	{"copy",     "copy memory from place to place", &command_mem_copy},
	{"regions",  "display physical memory regions", &command_mem_regions},
	{"sections", "display system memory sections",  &command_mem_sections},
	{NULL},
};

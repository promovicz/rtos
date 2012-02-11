
#include "commands.h"

#include <ctype.h>
#include <string.h>
#include <stdio.h>

#ifdef __arm__
#include <libdisarm/disarm.h>
#endif

static void hexdump(const uint8_t *data, unsigned int len)
{
	const uint8_t *bufptr = data;
	const uint8_t const *endptr = bufptr + len;
	unsigned int n, m, o, p;
	int hexchr;

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
		p = 71 - hexchr;
		for(o = 0; p < p; p++) {
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

static void disassemble(const uint32_t *base, unsigned int len)
{
	da_addr_t addr = ((da_addr_t)base);
	unsigned int n;

	for(n=0; n < len/4; n++) {
		da_word_t data = *((const uint32_t*)addr);

		da_instr_t instr;
		da_instr_args_t args;

		da_instr_parse(&instr, data, 0); // XXX 0 is little endian
		da_instr_parse_args(&args, &instr);

		printf("%08x\t", addr);
		printf("%08x\t", instr.data);
		da_instr_fprint(stdout, &instr, &args, addr);
		printf("\n");

		addr += sizeof(da_word_t);
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

int command_mem_disasm(struct cli *c, int argc, char **argv)
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
			len = 64;
		}
		// XXX align, foo!
		disassemble(addr, len);
		return 0;
	}

	printf("Usage: mem disasm <addr> [len]\n");

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

		printf("copying %ld bytes to %p from %p\n", len, (void*)dst, (void*)src);
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
		printf(" %s: %d bytes at %p - %p\n",
			   mr->name,
			   mr->size,
			   (void*)mr->start,
			   (void*)(((uintptr_t)mr->start) + mr->size - 1));
		mr++;
	}
	return 0;
}

int command_mem_sections(struct cli *c, int argc, char **argv)
{
	printf("system sections:\n");
	struct mem_section *ms = sections;
	while(ms->name) {
		printf(" %s: %zu bytes at %p - %p\n",
			   ms->name,
			   ((uintptr_t)ms->end) - ((uintptr_t)ms->start),
			   (void*)ms->start,
			   (void*)ms->end);
		ms++;
	}
	return 0;
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
	{.name = "dump",
	 .help = "dump memory at given address",
	 .handler = &command_mem_dump},
	{.name = "disasm",
	 .help = "disassemble code at given address",
	 .handler = &command_mem_disasm},
	{.name = "copy",
	 .help = "copy memory from place to place",
	 .handler = &command_mem_copy},
	{.name = "regions",
	 .help = "display physical memory regions",
	 .handler = &command_mem_regions},
	{.name = "sections",
	 .help = "display system memory sections",
	 .handler = &command_mem_sections},
};

DECLARE_COMMAND_TABLE(cmds_mem, cmd_mem);

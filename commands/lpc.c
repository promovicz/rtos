
#include "commands.h"

#include <stdio.h>

#include <lpc/vic.h>
#include <lpc/pll.h>
#include <lpc/ssp.h>
#include <lpc/spi.h>
#include <lpc/pinsel.h>

int command_lpc_pin(struct cli *c, int argc, char **argv)
{
	pin_report();
	return 0;
}

int command_lpc_pll(struct cli *c, int argc, char **argv)
{
	pll_report();
	return 0;
}

int command_lpc_vic(struct cli *c, int argc, char **argv)
{
	vic_report();
	return 0;
}

extern void csel_mmc(bool_t yeah);
extern void csel_scp(bool_t yeah);

int command_lpc_spi(struct cli *t, int argc, char **argv)
{
	int i;
	char *end;
	uint8_t rb, tb;
	bool_t flag;
	if(argc) {
		if(!strcmp("speak", argv[0])) {
			if(argc > 1) {
				printf("exchanging %d words:\n", argc - 1);
				csel_mmc(1);
				for(i = 1; i < argc; i++) {
					if(scan_byte(argv[i], &tb)) {
						rb = spi_transfer(tb);
						printf(" %02x -> %02x\n", tb, rb);
					} else {
						printf(" %s invalid\n", argv[i]);
						break;
					}
				}
				csel_mmc(0);
			}
		} else if (!strcmp("trace", argv[0])) {
			if(argc > 1) {
				if(scan_bool(argv[1], &flag)) {
					spi_enable_trace(flag);
				}
			} else {
				spi_enable_trace(BOOL_TRUE);
			}
		}
	}
	return 0;
}

int command_lpc_ssp(struct cli *t, int argc, char **argv)
{
	int i;
	char *end;
	uint8_t rb, tb;
	bool_t flag;
	if(argc) {
		if(!strcmp("speak", argv[0])) {
			if(argc > 1) {
				printf("exchanging %d words:\n", argc - 1);
				csel_scp(1);
				for(i = 1; i < argc; i++) {
					if(scan_byte(argv[i], &tb)) {
						rb = ssp_transfer(tb);
						printf(" %02x -> %02x\n", tb, rb);
					} else {
						printf(" %s invalid\n", argv[i]);
						break;
					}
				}
				csel_scp(0);
			}
		} else if (!strcmp("trace", argv[0])) {
			if(argc > 1) {
				if(scan_bool(argv[1], &flag)) {
					ssp_enable_trace(flag);
				}
			} else {
				ssp_enable_trace(BOOL_TRUE);
			}
		}
	}
	return 0;
}


struct command cmd_lpc[] = {
	{.name = "pin",
	 .help = "pin connect block",            
	 .handler = &command_lpc_pin},
	{.name = "pll",
	 .help = "phase-locked loops",
	 .handler = &command_lpc_pll},
	{.name = "vic",
	 .help = "vectored interrupt controller",
	 .handler = &command_lpc_vic},
	{.name = "spi",
	 .help = "standard spi interface",
	 .handler = &command_lpc_spi},
	{.name = "ssp",
	 .help = "fast spi interface",
	 .handler = &command_lpc_ssp},
};

DECLARE_COMMAND_TABLE(cmds_lpc, cmd_lpc);


#include "commands.h"

#include <lpc/vic.h>
#include <lpc/pll.h>
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

int command_lpc_spi(struct cli *c, int argc, char **argv)
{
	return 0;
}

int command_lpc_ssp(struct cli *c, int argc, char **argv)
{
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

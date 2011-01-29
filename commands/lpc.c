
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
	{"pin", "pin connect block",             &command_lpc_pin},
	{"pll", "phase-locked loops",            &command_lpc_pll},
	{"vic", "vectored interrupt controller", &command_lpc_vic},
	{"spi", "standard spi interface",        &command_lpc_spi},
	{"ssp", "fast spi interface",            &command_lpc_ssp},
	{NULL},
};

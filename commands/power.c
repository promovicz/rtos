
#include "commands.h"

#include <lpc/pcon.h>

void power_command(struct tty *t, int argc, char **argv)
{
	if(argc) {
		if((!strcmp("down", argv[0]))
		   || (!strcmp("off", argv[0]))) {
			pcon_power_down();
		}
	}
}

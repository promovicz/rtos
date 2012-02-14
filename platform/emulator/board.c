
#include <core/board.h>
#include <core/device.h>

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



void board_early_init(void)
{
}

void board_init(void)
{
	puts("Initializing board...");
}

void board_idle(void)
{
	sched_yield();
}

void board_reset(void)
{
	puts("Resetting.");
	exit(0);
}

void board_powerdown(void)
{
	puts("Powering down.");
	exit(0);
}


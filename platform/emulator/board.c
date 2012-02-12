
#include <core/board.h>
#include <core/device.h>

#include <stdio.h>
#include <unistd.h>

void board_idle(void)
{
	sched_yield();
}

void board_reset(void)
{
	emulator_puts("Resetting.");
	exit(0);
}

void board_powerdown(void)
{
	emulator_puts("Powering down.");
	exit(0);
}

void board_init(void)
{
	emulator_puts("Initializing board...");
}

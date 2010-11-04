
#include "commands.h"

#include <stdio.h>

#include <posix/file.h>
#include <posix/memory.h>

void posix_command(struct tty *t, int argc, char **argv)
{
	printf("posix emulator running\n");
	memory_report();
	file_table_report();
}

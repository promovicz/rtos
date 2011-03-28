
#include "commands.h"

#include <stdio.h>

#include <posix/file.h>
#include <posix/control.h>

int command_posix_status(struct cli *c, int argc, char **argv)
{
	printf("XXX\n");
	return 0;
}

int command_posix_memory(struct cli *c, int argc, char **argv)
{
	posix_memory_report();
	return 0;
}

int command_posix_file(struct cli *c, int argc, char **argv)
{
	file_table_report();
	return 0;
}

struct command cmd_posix[] = {
	{"status", "posix emulator status",   &command_posix_status},
	{"memory", "posix memory allocation", &command_posix_memory},
	{"file",   "posix file descriptors",  &command_posix_file},
	{NULL},
};

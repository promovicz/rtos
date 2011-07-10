
#include "commands.h"

#include <stdio.h>

#include <core/file.h>
#include <posix/control.h>

int command_posix_status(struct cli *c, int argc, char **argv)
{
	printf("XXX\n");
	return 0;
}

int command_posix_file(struct cli *c, int argc, char **argv)
{
	// XXX file_table_report();
	return 0;
}

struct command cmd_posix[] = {
	{.name = "status",
	 .help = "posix emulator status",
	 .handler = &command_posix_status},
	{.name = "file",
	 .help = "posix file descriptors",
	 .handler = &command_posix_file},
};

DECLARE_COMMAND_TABLE(cmds_posix, cmd_posix);

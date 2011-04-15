#ifndef CORE_CLI_H
#define CORE_CLI_H

#include <core/common.h>

struct cli;

typedef int (*command_handler_t) (struct cli *c, int argc, char **argv);

struct command {
	char *name;
	char *help;

	command_handler_t handler;
	struct command_table *children;
};

struct command_table {
	int cmdc;
	struct command *cmds;
};

#define DECLARE_COMMAND_TABLE(symbol, commands)	\
	struct command_table symbol = {				\
		.cmds = &commands[0],					\
		.cmdc = array_size(commands),			\
	}


struct cli {
	struct command_table *commands;
};

extern struct command_table cli_system_commands;

int cli_help(struct cli *c, int argc, char **argv);
int cli_execute(struct cli *c, int argc, char **argv);

#endif /* !CORE_CLI_H */

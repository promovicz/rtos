#ifndef CORE_CLI_H
#define CORE_CLI_H

#include <core/types.h>

struct cli {
	struct command *c_rootcmd;
};

typedef int (*command_handler_t) (struct cli *c, int argc, char **argv);

struct command {
	char *c_name;
	char *c_help;

	command_handler_t c_handler;
	struct command *c_children;
};

int cli_execute(struct cli *c, int argc, char **argv);

#endif /* !CORE_CLI_H */

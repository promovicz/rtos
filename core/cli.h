#ifndef CORE_CLI_H
#define CORE_CLI_H

#include <core/types.h>

struct cli {
	struct command *rootcmd;
};

typedef int (*command_handler_t) (struct cli *c, int argc, char **argv);

struct command {
	char *name;
	char *help;

	command_handler_t handler;
	struct command *children;
};

int cli_execute(struct cli *c, int argc, char **argv);

#endif /* !CORE_CLI_H */

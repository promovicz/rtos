
#include "cli.h"

#include <string.h>
#include <stdio.h>

static struct command *
cli_find(struct command_table *t, char *token)
{
	int i;

	for(i = 0; i < t->cmdc; i++) {
		struct command *c = t->cmds + i;
		if(!strcmp(token, c->name)) {
			fflush(stdout);
			return c;
		}
	}

	return NULL;
}

static void indent(int level)
{
	int i;
	for(i = 0; i < level; i++) {
		printf("  ");
	}
}

static void printtable(struct command_table *t, int level)
{
	int i;
	for(i = 0; i < t->cmdc; i++) {
		struct command *c = t->cmds + i;
		indent(level);
		printf("%s - %s\n", c->name, c->help);
	}
}

int cli_help(struct cli *c, int argc, char **argv)
{
	int i;
	struct command_table *t = c->commands;

	for(i = 0; i < argc; i++) {
		struct command *cmd = cli_find(t, argv[i]);
		if(cmd) {
			indent(i);
			printf("%s - %s\n", cmd->name, cmd->help);
			if(cmd->handler) {
				printf("command is complete.");
				return 0;
			} else if (cmd->children) {
				t = cmd->children;
			} else {
				printf("invalid command table entry.\n");
				return 1;
			}
		} else {
			indent(i);
			printf("%s unknown, options:\n", argv[i]);
			printtable(t, i + 1);
			return 1;
		}
	}

	indent(i);
	printf("options:\n");
	printtable(t, i + 1);

	return 1;
}

int cli_execute(struct cli *c, int argc, char **argv)
{
	int i;
	struct command_table *t = c->commands;

	for(i = 0; i < argc; i++) {
		struct command *cmd = cli_find(t, argv[i]);

		if(cmd) {
			if(cmd->handler) {
				return cmd->handler(c, argc - i - 1, argv + i + 1);
			} else if (cmd->children) {
				t = cmd->children;
			} else {
				printf("invalid command table entry.\n");
				return 1;
			}
		} else {
			printf("unknown command.\n");
			return 1;
		}
	}

	printf("incomplete command\n");
	return 1;
}


#include "cli.h"

#include <string.h>
#include <stdio.h>

#include <commands/commands.h>

static struct command system_commands[] = {
	{
		.name = "sys",
		.help = "system status and control",
		.handler = NULL,
		.children = &cmds_sys
	},
	{
		.name = "posix", 
		.help = "posix emulator",
		.handler = NULL,
		.children = &cmds_posix
	},
	{
		.name = "gpio",
		.help = "gpio pins",
		.handler = NULL,
		.children = &cmds_gpio
	},
	{
		.name = "mem",
		.help = "memory operations",
		.handler = NULL,
		.children = &cmds_mem
	},
	{
		.name = "lpc",
		.help = "lpc platform",
		.handler = NULL,
		.children = &cmds_lpc
	},
	{
		.name = "nmea",
		.help = "nmea node",
		.handler = NULL,
		.children = &cmds_nmea
	},
	{
		.name = "gps",
		.help = "gps receiver",
		.handler = NULL,
		.children = &cmds_gps
	},
};

DECLARE_COMMAND_TABLE(cli_system_commands, system_commands);

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

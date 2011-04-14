
#include "cli.h"

int cli_execute(struct cli *c, int argc, char **argv)
{
	int i;
	struct command *cmd = c->rootcmd;

	if(argc <= 0) {
		printf("No command given.\n");
		return 1;
	}

	for(i = 0; i <= argc; i++) {
		int found = 0;
		while(cmd->name) {
			if(i < argc) {
				if(!strcmp(cmd->name, argv[i])) {
					found = 1;
					break;
				}
			} else {
				if(!strlen(cmd->name)) {
					found = 1;
					break;
				}
			}
			cmd++;
		}
		if(found) {
			if(cmd->handler) {
				if(i < argc) {
					return cmd->handler(c, argc - i - 1, argv + i + 1);
				} else {
					return cmd->handler(c, 0, NULL);
				}
			} else if(cmd->children) {
				cmd = cmd->children;
			} else {
				printf("Stub command.\n");
				return 1;
			}
		} else {
			if(i < argc) {
				printf("Unknown command.\n");
			} else {
				printf("Incomplete command.\n");
			}
			return 1;
		}
	}

	printf("Unknown command.");
	return 1;
}


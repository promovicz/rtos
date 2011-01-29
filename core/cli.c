
#include "cli.h"

int cli_execute(struct cli *c, int argc, char **argv)
{
	int i;
	struct command *cmd = c->c_rootcmd;

	if(argc <= 0) {
		printf("No command given.\n");
		return 1;
	}

	for(i = 0; i <= argc; i++) {
		int found = 0;
		while(cmd->c_name) {
			if(i < argc) {
				if(!strcmp(cmd->c_name, argv[i])) {
					found = 1;
					break;
				}
			} else {
				if(!strlen(cmd->c_name)) {
					found = 1;
					break;
				}
			}
			cmd++;
		}
		if(found) {
			if(cmd->c_handler) {
				if(i < argc) {
					return cmd->c_handler(c, argc - i - 1, argv + i + 1);
				} else {
					return cmd->c_handler(c, 0, NULL);
				}
			} else if(cmd->c_children) {
				cmd = cmd->c_children;
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


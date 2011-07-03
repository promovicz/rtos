
#include <core/defines.h>

#include <core/system.h>

#include <sys/types.h>

#include <stdio.h>
#include <unistd.h>

void _exit(int status)
{
	printf("Exit with status %d\n", status);
	fflush(stdout);
	while(1) { }
}

void exit(int status)
	weak_alias("_exit");

pid_t getpid(void)
{
	return 0;
}

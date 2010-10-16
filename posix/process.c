
#include <core/defines.h>

#include <stdio.h>

void _exit(int status)
{
	printf("halted with status %d\n", status);
	fflush(stdout);
	while(1) {}
}

void exit(int status)
	weak_alias("_exit");

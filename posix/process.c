
#include <core/defines.h>

#include <core/system.h>

#include <sys/types.h>

#include <stdio.h>
#include <unistd.h>

void _exit(int status)
{
	nanosecs_t waitstart = system_get_time();
	printf("Exit with status %d\n", status);
	fflush(stdout);
	while((system_get_time() - waitstart) < NANOSECS_SEC) {
		system_kick();
		system_idle();
	}
	while(1) {
	}
}

void exit(int status)
	weak_alias("_exit");

pid_t getpid(void)
{
	return 0;
}

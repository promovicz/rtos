
#include "control.h"

void posix_init(void)
{
	posix_memory_init();

	posix_console_init();
}

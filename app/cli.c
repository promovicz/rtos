
#include <core/system.h>
#include <core/device.h>
#include <core/cli.h>

#include <stdio.h>

struct tty t;
struct cli c;

int main(void)
{
	system_init();

	device_open("vcom", O_RDONLY);
	device_open("vcom", O_WRONLY);
	device_open("vcom", O_WRONLY);

	tty_init(&t, 0, 1);
	cli_init(&c, &t);
	
	while (1) {
		system_kick();

		tty_process(&t);

		fflush(stdout);

		system_idle();
	}

	return 0;
}

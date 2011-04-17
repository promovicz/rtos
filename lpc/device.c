
#include "device.h"

const struct lpc_device devs[] = {
	{"ad0"},
	{"ad1"},
	{"da"},
	{"eint"},
	{"gpio"},
	{"i2c0"},
	{"i2c1"},
	{"pwm"},
	{"spi"},
	{"ssp"},
	{"timer0"},
	{"timer1"},
	{"uart0"},
	{"uart1"},
	{"usb"},
};

const char *device_name(int device)
{
	if(device == DEVICE_NONE) {
		return "none";
	} else {
		return devs[device].name;
	}
}

#ifndef LPC_DEVICE_H
#define LPC_DEVICE_H

struct lpc_device {
	char *name;
};

enum {
	DEVICE_AD0 = 0,
	DEVICE_AD1,
	DEVICE_DA,
	DEVICE_EINT,
	DEVICE_GPIO,
	DEVICE_I2C0,
	DEVICE_I2C1,
	DEVICE_PWM,
	DEVICE_SPI,
	DEVICE_SSP,
	DEVICE_TIMER0,
	DEVICE_TIMER1,
	DEVICE_UART0,
	DEVICE_UART1,
	DEVICE_USB,

	NUM_DEVICE,

	DEVICE_NONE = -1,
};

const char *device_name(int device);

#endif /* !LPC_DEVICE_H */

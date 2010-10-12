
#include "pin.h"

const struct pin pins[] = {
	{"p0.0",  {{PIN_FUNCTION_GPIO_0, DEVICE_GPIO, "0"},
			   {PIN_FUNCTION_UART0_TXD, DEVICE_UART0, "txd"},
			   {PIN_FUNCTION_PWM_1, DEVICE_PWM, "1"},
			   {PIN_FUNCTION_NONE, DEVICE_NONE, NULL}}},
	{"p0.1",  {{PIN_FUNCTION_GPIO_1, DEVICE_GPIO, "1"},
			   {PIN_FUNCTION_UART0_RXD, DEVICE_UART0, "rxd"},
			   {PIN_FUNCTION_PWM_3, DEVICE_PWM, "3"},
			   {PIN_FUNCTION_EINT_0, DEVICE_EINT, "0"}}},
	{"p0.2",  {{PIN_FUNCTION_GPIO_2, DEVICE_GPIO, "2"},
			   {PIN_FUNCTION_I2C0_SCL, DEVICE_I2C0, "scl"},
			   {PIN_FUNCTION_TIMER0_C0, DEVICE_TIMER0, "c0"},
			   {PIN_FUNCTION_NONE, DEVICE_NONE, NULL}}},
	{"p0.3",  {{PIN_FUNCTION_GPIO_3, DEVICE_GPIO, "3"},
			   {PIN_FUNCTION_I2C0_SDA, DEVICE_I2C0, "sda"},
			   {PIN_FUNCTION_TIMER0_M0, DEVICE_TIMER0, "m0"},
			   {PIN_FUNCTION_EINT_1, DEVICE_EINT, "1"}}},
	{"p0.4",  {{PIN_FUNCTION_GPIO_4, DEVICE_GPIO, "4"},
			   {PIN_FUNCTION_SPI_SCK, DEVICE_SPI, "sck"},
			   {PIN_FUNCTION_TIMER0_C1, DEVICE_TIMER0, "c1"},
			   {PIN_FUNCTION_AD0_6, DEVICE_AD0, "6"}}},
	{"p0.5",  {{PIN_FUNCTION_GPIO_5, DEVICE_GPIO, "5"},
			   {PIN_FUNCTION_SPI_MISO, DEVICE_SPI, "miso"},
			   {PIN_FUNCTION_TIMER0_M1, DEVICE_TIMER0, "m1"},
			   {PIN_FUNCTION_AD0_7, DEVICE_AD0, "7"}}},
	{"p0.6",  {{PIN_FUNCTION_GPIO_6, DEVICE_GPIO, "6"},
			   {PIN_FUNCTION_SPI_MOSI, DEVICE_SPI, "mosi"},
			   {PIN_FUNCTION_TIMER0_C2, DEVICE_TIMER0, "c2"},
			   {PIN_FUNCTION_AD1_0, DEVICE_AD1, "0"}}},
	{"p0.7",  {{PIN_FUNCTION_GPIO_7, DEVICE_GPIO, "7"},
			   {PIN_FUNCTION_SPI_SSEL, DEVICE_SPI, "ssel"},
			   {PIN_FUNCTION_PWM_2, DEVICE_PWM, "2"},
			   {PIN_FUNCTION_EINT_2, DEVICE_EINT, "2"}}},
	{"p0.8",  {{PIN_FUNCTION_GPIO_8, DEVICE_GPIO, "8"},
			   {PIN_FUNCTION_UART1_TXD, DEVICE_UART1, "txd"},
			   {PIN_FUNCTION_PWM_4, DEVICE_PWM, "4"},
			   {PIN_FUNCTION_AD1_1, DEVICE_AD1, "1"}}},
	{"p0.9",  {{PIN_FUNCTION_GPIO_9, DEVICE_GPIO, "9"},
			   {PIN_FUNCTION_UART1_RXD, DEVICE_UART1, "rxd"},
			   {PIN_FUNCTION_PWM_6, DEVICE_PWM, "6"},
			   {PIN_FUNCTION_EINT_3, DEVICE_EINT, "3"}}},
	{"p0.10", {{PIN_FUNCTION_GPIO_10, DEVICE_GPIO, "10"},
			   {PIN_FUNCTION_UART1_RTS, DEVICE_UART1, "rts"},
			   {PIN_FUNCTION_TIMER1_C0, DEVICE_TIMER1, "c0"},
			   {PIN_FUNCTION_AD1_2, DEVICE_AD1, "2"}}},
	{"p0.11", {{PIN_FUNCTION_GPIO_11, DEVICE_GPIO, "11"},
			   {PIN_FUNCTION_UART1_CTS, DEVICE_UART1, "cts"},
			   {PIN_FUNCTION_TIMER1_C1, DEVICE_TIMER1, "c1"},
			   {PIN_FUNCTION_I2C1_SCL, DEVICE_I2C1, "scl"}}},
	{"p0.12", {{PIN_FUNCTION_GPIO_12, DEVICE_GPIO, "12"},
			   {PIN_FUNCTION_UART1_DSR, DEVICE_UART1, "dsr"},
			   {PIN_FUNCTION_TIMER1_M0, DEVICE_TIMER1, "m0"},
			   {PIN_FUNCTION_AD1_3, DEVICE_AD1, "3"}}},
	{"p0.13", {{PIN_FUNCTION_GPIO_13, DEVICE_GPIO, "13"},
			   {PIN_FUNCTION_UART1_DTR, DEVICE_UART1, "dtr"},
			   {PIN_FUNCTION_TIMER1_M1, DEVICE_TIMER1, "m1"},
			   {PIN_FUNCTION_AD1_4, DEVICE_AD1, "4"}}},
	{"p0.14", {{PIN_FUNCTION_GPIO_14, DEVICE_GPIO, "14"},
			   {PIN_FUNCTION_UART1_DCD, DEVICE_UART1, "dcd"},
			   {PIN_FUNCTION_EINT_1, DEVICE_EINT, "1"},
			   {PIN_FUNCTION_I2C1_SDA, DEVICE_I2C1, "sda"}}},
	{"p0.15", {{PIN_FUNCTION_GPIO_15, DEVICE_GPIO, "15"},
			   {PIN_FUNCTION_UART1_RI, DEVICE_UART1, "ri"},
			   {PIN_FUNCTION_EINT_2, DEVICE_EINT, "2"},
			   {PIN_FUNCTION_AD1_5, DEVICE_AD1, "5"}}},
	{"p0.16", {{PIN_FUNCTION_GPIO_16, DEVICE_GPIO, "16"},
			   {PIN_FUNCTION_EINT_0, DEVICE_EINT, "0"},
			   {PIN_FUNCTION_TIMER0_M2, DEVICE_TIMER0, "m2"},
			   {PIN_FUNCTION_TIMER0_C2, DEVICE_TIMER0, "c2"}}},
	{"p0.17", {{PIN_FUNCTION_GPIO_17, DEVICE_GPIO, "17"},
			   {PIN_FUNCTION_TIMER1_C2, DEVICE_TIMER1, "c2"},
			   {PIN_FUNCTION_SSP_SCK, DEVICE_SSP, "sck"},
			   {PIN_FUNCTION_TIMER1_M2, DEVICE_TIMER1, "m2"}}},
	{"p0.18", {{PIN_FUNCTION_GPIO_18, DEVICE_GPIO, "18"},
			   {PIN_FUNCTION_TIMER1_C3, DEVICE_TIMER1, "c3"},
			   {PIN_FUNCTION_SSP_MISO, DEVICE_SSP, "miso"},
			   {PIN_FUNCTION_TIMER1_M3, DEVICE_TIMER1, "m3"}}},
	{"p0.19", {{PIN_FUNCTION_GPIO_19, DEVICE_GPIO, "19"},
			   {PIN_FUNCTION_TIMER1_M2, DEVICE_TIMER1, "m2"},
			   {PIN_FUNCTION_SSP_MOSI, DEVICE_SSP, "mosi"},
			   {PIN_FUNCTION_TIMER1_C2, DEVICE_TIMER1, "c2"}}},
	{"p0.20", {{PIN_FUNCTION_GPIO_20, DEVICE_GPIO, "20"},
			   {PIN_FUNCTION_TIMER1_M3, DEVICE_TIMER1, "m3"},
			   {PIN_FUNCTION_SSP_SSEL, DEVICE_SSP, "ssel"},
			   {PIN_FUNCTION_EINT_3, DEVICE_EINT, "3"}}},
	{"p0.21", {{PIN_FUNCTION_GPIO_21, DEVICE_GPIO, "21"},
			   {PIN_FUNCTION_PWM_5, DEVICE_PWM, "5"},
			   {PIN_FUNCTION_AD1_6, DEVICE_AD1, "6"},
			   {PIN_FUNCTION_TIMER1_C3, DEVICE_TIMER1, "c3"}}},
	{"p0.22", {{PIN_FUNCTION_GPIO_22, DEVICE_GPIO, "22"},
			   {PIN_FUNCTION_AD1_7, DEVICE_AD1, "7"},
			   {PIN_FUNCTION_TIMER0_C0, DEVICE_TIMER0, "c0"},
			   {PIN_FUNCTION_TIMER0_M0, DEVICE_TIMER0, "m0"}}},
	{"p0.23", {{PIN_FUNCTION_GPIO_23, DEVICE_GPIO, "23"},
			   {PIN_FUNCTION_NONE, DEVICE_NONE, "vbus"},
			   {PIN_FUNCTION_NONE, DEVICE_NONE, NULL},
			   {PIN_FUNCTION_NONE, DEVICE_NONE, NULL}}},
	{"p0.24", {{PIN_FUNCTION_NONE, DEVICE_NONE, NULL},
			   {PIN_FUNCTION_NONE, DEVICE_NONE, NULL},
			   {PIN_FUNCTION_NONE, DEVICE_NONE, NULL},
			   {PIN_FUNCTION_NONE, DEVICE_NONE, NULL}}},
	{"p0.25", {{PIN_FUNCTION_GPIO_25, DEVICE_GPIO, "25"},
			   {PIN_FUNCTION_AD0_4, DEVICE_AD0, "4"},
			   {PIN_FUNCTION_NONE, DEVICE_NONE, "aout"},
			   {PIN_FUNCTION_NONE, DEVICE_NONE, NULL}}},
	{"p0.26", {{PIN_FUNCTION_NONE, DEVICE_NONE, NULL},
			   {PIN_FUNCTION_NONE, DEVICE_NONE, NULL},
			   {PIN_FUNCTION_NONE, DEVICE_NONE, NULL},
			   {PIN_FUNCTION_NONE, DEVICE_NONE, NULL}}},
	{"p0.27", {{PIN_FUNCTION_NONE, DEVICE_NONE, NULL},
			   {PIN_FUNCTION_NONE, DEVICE_NONE, NULL},
			   {PIN_FUNCTION_NONE, DEVICE_NONE, NULL},
			   {PIN_FUNCTION_NONE, DEVICE_NONE, NULL}}},
	{"p0.28", {{PIN_FUNCTION_GPIO_28, DEVICE_GPIO, "28"},
			   {PIN_FUNCTION_AD0_1, DEVICE_AD0, "1"},
			   {PIN_FUNCTION_TIMER0_C2, DEVICE_TIMER0, "c2"},
			   {PIN_FUNCTION_TIMER0_M2, DEVICE_TIMER0, "m2"}}},
	{"p0.29", {{PIN_FUNCTION_GPIO_29, DEVICE_GPIO, "29"},
			   {PIN_FUNCTION_AD0_2, DEVICE_AD0, "2"},
			   {PIN_FUNCTION_TIMER0_C3, DEVICE_TIMER0, "c3"},
			   {PIN_FUNCTION_TIMER0_M3, DEVICE_TIMER0, "m3"}}},
	{"p0.30", {{PIN_FUNCTION_GPIO_30, DEVICE_GPIO, "30"},
			   {PIN_FUNCTION_AD0_3, DEVICE_AD0, "3"},
			   {PIN_FUNCTION_EINT_3, DEVICE_EINT, "3"},
			   {PIN_FUNCTION_TIMER0_C0, DEVICE_TIMER0, "c0"}}},
	{"p0.31", {{PIN_FUNCTION_GPIO_31, DEVICE_GPIO, "31"},
			   {PIN_FUNCTION_NONE, DEVICE_NONE, "upled"},
			   {PIN_FUNCTION_NONE, DEVICE_NONE, "connect"},
			   {PIN_FUNCTION_NONE, DEVICE_NONE, NULL}}},
};
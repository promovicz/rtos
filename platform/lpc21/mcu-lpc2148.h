#ifndef LPC21_MCU_LPC2148_H
#define LPC21_MCU_LPC2148_H

/* Number of vectors in VIC */
#define VIC_VECTOR_COUNT 16

/* Interrupt channel enum for VIC */
enum vic_int {
	INT_WDT = 0,
	INT_SWI,
	INT_DbgCommRx,
	INT_DbgCommTx,
	INT_TIMER0,
	INT_TIMER1,
	INT_UART0,
	INT_UART1,
	INT_PWM0,
	INT_I2C0,
	INT_SPI0,
	INT_SPI1,
	INT_PLL,
	INT_RTC,
	INT_EINT0,
	INT_EINT1,
	INT_EINT2,
	INT_EINT3,
	INT_ADC0,
	INT_I2C1,
	INT_BOD,
	INT_ADC1,
	INT_USB,
	VIC_INT_COUNT
};

/* Interrupt names for VIC */
char *vic_int_names[VIC_INT_COUNT];

#endif /* !LPC21_MCU_LPC2148_H */

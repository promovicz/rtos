ENTRY(_startup)
MEMORY 
{
	flash     			: ORIGIN = 0,          LENGTH = 512K	/* FLASH ROM                            	*/	
	ram_isp_low(A)		: ORIGIN = 0x40000120, LENGTH = 223		/* variables used by Philips ISP bootloader	*/		 
	ram   				: ORIGIN = 0x40000200, LENGTH = 32513   /* free RAM area							*/
	ram_isp_high(A)		: ORIGIN = 0x40007FE0, LENGTH = 32		/* variables used by Philips ISP bootloader	*/
	ram_usb_dma			: ORIGIN = 0x7FD00000, LENGTH = 8192    /* on-chip USB DMA RAM area (not used)      */
}
SECTIONS 
{

	/* FLASH */
	. = ORIGIN(flash);

	.text :
	{
		*(.startup)
		*(.text) *(.text.*)
		*(.rodata) *(.rodata.*)
		*(.glue_7)
		*(.glue_7t)
		_etext = .;
	} > flash

	/* USB DMA RAM */
	. = ORIGIN(ram_usb_dma);

	.usbdma (NOLOAD) :
	{
		*(.usbdma)
	} > ram_usb_dma

	 /* RAM */
	 . = ORIGIN(ram);

	 __static_memory_start = .;

	.data :
	{
		_data = .;
		*(.data) *(.data.*)
		_edata = .;
	} > ram AT > flash

	.bss (NOLOAD) :
	{
		__bss_start = .;
		*(.bss)
	} > ram
	. = ALIGN(4);
	__bss_end = . ;

	__static_memory_end = .;

	. = ORIGIN(ram) + (LENGTH(ram) & ~ 3); /* XXX strange memdefs have unaligned memory size */
	__stacks_end = .;
	__stacks_start = __stacks_end - 4096;

	__dynamic_memory_start = __static_memory_end;
	__dynamic_memory_end = __stacks_start;
}

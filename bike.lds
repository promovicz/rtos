ENTRY(_startup)
MEMORY 
{
    flash     	        : ORIGIN = 0,          LENGTH = 512K    /* FLASH ROM                                */
    ram                 : ORIGIN = 0x40000200, LENGTH = 32000   /* free RAM area                            */
    ram_usbdma          : ORIGIN = 0x7FD00000, LENGTH = 8192    /* on-chip USB DMA RAM area (not used)      */
}
SECTIONS 
{

	/* FLASH */
	. = ORIGIN(flash);

	.text :
	{
		__text_start = .;
		*(.startup)
		*(.text) *(.text.*)
		*(.rodata) *(.rodata.*)
		*(.glue_7)
		*(.glue_7t)
		_etext = .;
	} > flash
	. = ALIGN(4);
	__text_end = .;

	 /* RAM */
	 . = ORIGIN(ram);

	 __static_start = .;

	.data :
	{
		__data_start = .;
		_data = .;
		*(.data) *(.data.*)
		_edata = .;
	} > ram AT > flash
	. = ALIGN(4);
	__data_end = .;

	.bss (NOLOAD) :
	{
		__bss_start = .;
		*(.bss)
	} > ram
	. = ALIGN(4);
	__bss_end = . ;

	__static_end = .;

	. = ORIGIN(ram) + LENGTH(ram);
	__stacks_end = .;
	__stacks_start = __stacks_end - 4096;

	__heap_start = __static_end;
	__heap_end = __stacks_start;

	/* USB DMA RAM */
	. = ORIGIN(ram_usb_dma);

	.usbdma (NOLOAD) :
	{
		*(.usbdma)
	} > ram_usb_dma

}

ENTRY(_entry)
MEMORY 
{
    flash     	        : ORIGIN = 0x00010000, LENGTH = 448K    /* FLASH ROM                                */
    ram                 : ORIGIN = 0x40000000, LENGTH = 32K     /* free RAM area                            */
    ram_usbdma          : ORIGIN = 0x7FD00000, LENGTH = 8K      /* on-chip USB DMA RAM area (not used)      */
}
SECTIONS 
{

	/* FLASH */
	. = ORIGIN(flash);

	.text :
	{
		__text_start = .;
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
		*(.startup)
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
	. = ORIGIN(ram_usbdma);

	.usbdma (NOLOAD) :
	{
		*(.usbdma)
	} > ram_usbdma

}

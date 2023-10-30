
INCLUDE patch_link_location.ld

/* Memory configuration test */

MEMORY
{
    rom     :   ORIGIN = 0x10000000, LENGTH = 80k
    otp     :   ORIGIN = 0x10020000, LENGTH = 64k
    ram     :   ORIGIN = ramstartaddr, LENGTH = ramsize
}

_otp_patch_header_size = 28;

/* The partition sizes are defined by trimming and need to be aligned 
   with the trim data */
_otp_data_size   = 16384;
_otp_patch1_size = 16384;
_otp_patch2_size = 16384;
_otp_patch3_size = 16384;
_otp_patch4_size = 0;

SECTIONS {
    
	/* Reseved space for data.
	/* These data must be intialized by the patch itself if needed */
	.data :
    {
       _start_data = .;
        *(.data*)
       _end_data = .;	

    } >ram
		
    . = ALIGN(4);
	
	/* Reseved space for zero initialized data.
	/* These data must be intialized by the patch itself if needed */
    .bss :
    {
      _start_bss = .;
        *(.bss*)
        *(COMMON)
      _end_bss = .;
		
    } >ram

    . = ALIGN(4);

   /* Space for Reset resistant variables */
   .reset_resist :
    {

      *(RESET_RESIST)

    } >ram

		
	/* Space for functions which should be executed in ram

    . = ALIGN(4);
    
    /* Reserved space for OTP data region which contains configuration data */	
	.otp_data :
    {
	  _otp_data_base = .;
	  . += _otp_data_size;
	  _end_otp_data = .;
	
    } >otp

    /* Reserved space for the patch header.
	   The patch region 1 follows the data region */	
	.otp_header :
	{
	  . += _otp_patch_header_size;
	} >otp
	
	/* Reserved space for the patch code */	
	.txt :
    {
	    *(.text*)
		*(.rodata*)
		*(.ARM.extab* .gnu.linkonce.armextab.*)
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
      _end_otp_patch1 = .;	
	  _end_text = .;
    } >otp

	/* Reserved space for functions to be executed in RAM instead of OTP */
	_ram_exec_source = ALIGN (4);

	.ram_exec : AT (_ram_exec_source)
	{
		_start_ram_exec = .;
		*(RAM_EXEC*)
		_end_ram_exec = .;	   
	} > ram

    . = ALIGN(4);
	
	/* These are examples for additional patch partitions 
	   They only need to be defined if needed. */
	.otp_patch2 :
    {	
	  _otp_patch2_base = .;
	  . += _otp_patch2_size;
      _end_otp_patch2 = .;	
    } >otp

	.otp_patch3 :
    {	
	  _otp_patch3_base = .;
	  . += _otp_patch3_size;
      _end_otp_patch3 = .;	
    } >otp

	.otp_patch4 :
    {	
	  _otp_patch4_base = .;
	  . += _otp_patch4_size;
      _end_otp_patch4 = .;	
    } >otp

}

_end = .;
PROVIDE(end = .);

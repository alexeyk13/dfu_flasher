
MEMORY
{
 FLASH (rx) : ORIGIN = 0x0800A000, LENGTH = 0x10000000
 RAM (rw) : ORIGIN = 0x20000000, LENGTH = 0x10000000
}

SECTIONS
{
 .text :
 {
  KEEP(*(.loader));
  *(.text .text.* .gnu.linkonce.t.*)
  *(.glue_7t) *(.glue_7)
  *(.rodata .rodata.* .gnu.linkonce.r.*)
  . = ALIGN(4);
  _etext = .;
 } > FLASH

 _end = . ;
}

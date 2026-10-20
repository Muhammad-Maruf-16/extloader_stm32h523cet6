ENTRY(Init)

MEMORY
{
  Dev_Info (r)  : ORIGIN = 0x00000000, LENGTH = 512
  RAM (rwx)     : ORIGIN = 0x20000004, LENGTH = 64K
}

SECTIONS
{
  .Dev_Info :
  {
    KEEP(*(.Dev_Info))
  } > Dev_Info

  .text :
  {
    . = ALIGN(4);
    KEEP(*(.text))
    KEEP(*(.text*))
    KEEP(*(.rodata))
    KEEP(*(.rodata*))
    . = ALIGN(4);
  } > RAM

  .data :
  {
    . = ALIGN(4);
    *(.data)
    *(.data*)
    . = ALIGN(4);
  } > RAM

  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } > RAM

  /DISCARD/ :
  {
    *(.ARM.exidx*)
    *(.ARM.extab*)
  }
}

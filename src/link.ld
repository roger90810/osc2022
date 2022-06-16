SECTIONS
{
    . = 0xFFFF000000000000;
    . += 0x80000;
    __kernel_start = .;
    .text : { KEEP(*(.text.boot)) *(.text .text.*) }
    .rodata : { *(.rodata .rodata.*) }
    .data : { *(.data .data.*) }
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        __bss_end = .;
    }
    _end = .;
    __heap_start = .;
}
__heap_size = 0x00005000;
__bss_size = (__bss_end - __bss_start)>>3;
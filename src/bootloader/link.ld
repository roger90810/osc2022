SECTIONS
{
    . = 0x60000;

    __code_start = .;
    .text : { KEEP(*(.text.boot)) *(.text .text.*) }
    .rodata : { *(.rodata .rodata.*) }
    .data : { *(.data .data.*) }
    __code_end = .;

    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        __bss_end = .;
    }
    _end = .;
}
__code_size = (__code_end - __code_start) >> 3;
__bss_size = (__bss_end - __bss_start) >> 3;
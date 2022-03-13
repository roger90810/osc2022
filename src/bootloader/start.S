.section ".text.boot"

.global _start

_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f
    // cpu id > 0, stop
1:  wfe
    b       1b
2:  // cpu id == 0

    ldr     x1, =0x80000
    ldr     x2, =_start
    mov     sp, x2 // set stack pointer to top
    ldr     w3, =__code_size
relocate_loop:
    ldr     x4, [x1], #8
    str     x4, [x2], #8
    sub     w3, w3, #1
    cbnz    w3, relocate_loop

    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
    cbz     w2, 4f
clear_bss_loop:
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, clear_bss_loop

    // jump to C code, should not return
4:  bl      boot
    // for failsafe, halt this core too
    b       1b
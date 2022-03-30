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
    bl from_el2_to_el1
    // after switch to el1, set the address of exception vertor table. 
    adr x1, exception_vector_table
    msr vbar_el1, x1
    // set top of stack just before our code (stack grows to a lower address per AAPCS64)
    ldr     x1, =_start
    mov     sp, x1

    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b
    

    // jump to C code, should not return
4:  bl      main
    // for failsafe, halt this core too
    b       1b

from_el2_to_el1 :
    mov x2, (1 << 31) // EL1 uses aarch64
    msr hcr_el2, x2
    mov x2, 0x345     // EL1h (SPSel = 1) with interrupt disabled
    msr spsr_el2, x2
    msr elr_el2, lr
    eret // return to EL1
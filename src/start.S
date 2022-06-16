#include "mmu.h"
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

    bl set_mmu
    
    // set top of stack just before our code (stack grows to a lower address per AAPCS64)
    ldr     x1, =__kernel_start
    mov     sp, x1
    
    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b
    
    // jump to C code, should not return
4:  ldr     x1, =main
    br      x1
    // for failsafe, halt this core too
    b       1b

from_el2_to_el1 :
    mov x2, (1 << 31) // EL1 uses aarch64
    msr hcr_el2, x2
    mov x2, 0x345     // EL1h (SPSel = 1) with interrupt disabled
    msr spsr_el2, x2
    msr elr_el2, lr
    eret // return to EL1

set_mmu :
    ldr x1, = TCR_CONFIG_DEFAULT
    msr tcr_el1, x1
    
    ldr x1, = MAIR_EL1_VALUE
    msr mair_el1, x1

    mov x1, 0 // PGD's page frame at 0x0
    mov x2, 0x1000 // PUD's page frame at 0x1000

    ldr x3, = BOOT_PGD_ATTR
    orr x3, x2, x3 // combine the physical address of next level page with attribute.
    str x3, [x0]

    ldr x3, = BOOT_PUD_ATTR
    mov x4, 0x00000000
    orr x4, x3, x4
    str x4, [x2] // 1st 1GB mapped by the 1st entry of PUD
    mov x4, 0x40000000
    orr x4, x3, x4
    str x4, [x2, 8] // 2nd 1GB mapped by the 2nd entry of PUD

    msr ttbr0_el1, x1 // load PGD to the bottom translation-based register.
    // msr ttbr1_el1, x1 // also load PGD to the upper translation based register.

    mrs x3, sctlr_el1 // System Control Register
    orr x3, x3, 1     // M, bit [0]. EL1&0 stage 1 address translation enabled.
    msr sctlr_el1, x3 // enable MMU, cache remains disabled

    ret
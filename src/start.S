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

    ldr x1, =pg_dir
    and x1, x1, 0x0000ffffffffffff // PGD (Use physical address)
    add x2, x1, 0x1000  // PUD
    add x3, x2, 0x1000  // PMD
    add x4, x3, 0x1000  // PTE

    /* setup PGD entry 0 */
    ldr x5, =PGD0_ATTR
    orr x5, x2, x5
    str x5, [x1]

    /* setup PUD entry 0 */
    ldr x5, =PUD0_ATTR
    orr x5, x3, x5
    str x5, [x2]

    /* setup PUD entry 1 (point to a block, 1GB section start from 0x4000_0000) */
    ldr x5, =PUD1_ATTR
    mov x6, 0x40000000
    orr x5, x6, x5
    str x5, [x2, #8]

    /* setup PMD entry 0 ~ 512 */
    mov x5, x4 // point to current PTE address
    mov x6, xzr
    mov x7, #512
set_pmd_loop :
    ldr x8, =PMD_ATTR
    orr x8, x5, x8
    str x8, [x3, x6, lsl #3]
    add x6, x6, #1
    add x5, x5, 0x1000
    cmp x6, x7
    bls set_pmd_loop

    /* setup PTE, 512 pages, 512 entries per page */

    /* normal PTE total : 512 * 512 - 4096 = 258048 entries */
    mov x5, xzr
    mov x6, xzr
    mov x7, #258048
set_normal_pte_loop :
    ldr x8, =PTE_NORMAL_ATTR
    orr x8, x5, x8
    str x8, [x4, x6, lsl #3]
    add x6, x6, #1
    add x5, x5, 0x1000
    cmp x6, x7
    bls set_normal_pte_loop


    /* device PTE total : 16MB = 4096 entries */
    add x7, x7, #4096
set_device_pte_loop :
    ldr x8, =PTE_DEVICE_ATTR
    orr x8, x5, x8
    str x8, [x4, x6, lsl #3]
    add x6, x6, #1
    add x5, x5, 0x1000
    cmp x6, x7
    bls set_device_pte_loop

    msr ttbr0_el1, x1 // load PGD to the bottom translation-based register.
    msr ttbr1_el1, x1 // also load PGD to the upper translation based register.

    /* enable MMU */
    mrs x3, sctlr_el1 // System Control Register
    orr x3, x3, 1     // M, bit [0]. EL1&0 stage 1 address translation enabled.
    msr sctlr_el1, x3 // enable MMU, cache remains disabled

    ret
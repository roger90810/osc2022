#include "../uart.h"

int boot()
{
    char *kernel_addr = (char *)0x80000;
    volatile uint64_t dtb_base = 0;
    asm volatile("mov %0, x0" : "=r"(dtb_base));
    uart_init();
    // uart_puts("Waiting for kernel image...\n");
    
    unsigned int kernel_size = 0;
    for (int i = 0; i < 4; i++) {
        kernel_size <<= 8;
        kernel_size |= uart_getc();
    }

    // uart_puts("The kernel Size is : ");
    // uart_putx(kernel_size);
    // uart_puts("\n");

    for (int i = 0; i < kernel_size; i++) {
        kernel_addr[i] = uart_getc();   
    }
    // uart_puts("Read kernel done !! \n");
    asm volatile("mov x0, %0" :: "r"(dtb_base));
    asm volatile (
        // we must force an absolute address to branch to
        "mov x30, 0x80000; ret" // X30 is used as the Link Register and can be referred to as LR.
        // "ret x30" // Return from subroutine, branches unconditionally to an address in a register, with a hint that this is a subroutine return.
    );
    
    return 0;
}
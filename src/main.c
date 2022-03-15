#include "uart.h"
#include "shell.h"
#include "stdint.h"
#include "dtb.h"

uint64_t DTB_BASE;
int main()
{
    // register uint64_t x0 asm("x0");  // clang does not support register variable??
    asm volatile("mov %0, x0" : "=r"(DTB_BASE));
    uart_init();
    start_shell();
    return 0;
}
#include "uart.h"
#include "shell.h"
#include "types.h"
#include "dtb.h"
#include "mm.h"

uint64_t DTB_BASE;
uint32_t CPIO_BASE;
void initramfs_callback(uint8_t* addr, uint32_t len)
{
    for (int i = 0; i < len; i++) {
        CPIO_BASE <<= 8;
        CPIO_BASE |= (addr[i] & 0xff);
    }
}

int main()
{
    asm volatile("mov %0, x0" : "=r"(DTB_BASE));
    uart_init();
    void (*ptr)(uint8_t* , uint32_t) = &initramfs_callback;
    dtb_parser(DTB_BASE, ptr);
    mm_init();
    timer_init();
    start_shell();
    return 0;
}
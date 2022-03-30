#include "stdint.h"
extern uint64_t start_time;
void exception_entry()
{
    volatile uint64_t tmp;
    asm volatile("mrs %0, spsr_el1" : "=r"(tmp)); 
    uart_puts("The value of spsr_el1 : ");
    uart_putx(tmp);
    uart_puts("\n");

    asm volatile("mrs %0, elr_el1" : "=r"(tmp)); 
    uart_puts("The value of elr_el1 : ");
    uart_putx(tmp);
    uart_puts("\n");

    asm volatile("mrs %0, esr_el1" : "=r"(tmp)); 
    uart_puts("The value of esr_el1 : ");
    uart_putx(tmp);
    uart_puts("\n");
}

void irq_handler()
{
    volatile uint64_t curr_time;
    volatile uint64_t freq;
    asm volatile("mrs %0, cntpct_el0" : "=r"(curr_time));
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));

    uart_puts("Seconds after booting : ");
    uart_putx((curr_time - start_time) / freq);
    uart_puts("\n");

    freq <<= 1;
    asm volatile("msr cntp_tval_el0, %0" :: "r"(freq));
}
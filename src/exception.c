#include "exception.h"

void enable_timer_interrupt(const uint8_t enable)
{
    if (enable) {
        // enable timer irq
        // enable core timer
        volatile uint64_t tmp = 0x1;
        asm volatile("msr cntp_ctl_el0, %0" :: "r"(tmp)); // enable timer interrupt
        tmp = CORE0_TIMER_IRQCNTL;
        asm volatile("mov x1, %0" :: "r"(tmp));
        tmp = 0x2;
        asm volatile("mov x0, %0" :: "r"(tmp));
        asm volatile("str w0, [x1]");  // unmask timer interrupt
    } else {
        // disable timer irq
        // disable core timer
        volatile uint64_t tmp = 0x0;
        asm volatile("msr cntp_ctl_el0, %0" :: "r"(tmp)); // disable timer interrupt
        tmp = CORE0_TIMER_IRQCNTL;
        asm volatile("mov x1, %0" :: "r"(tmp));
        tmp = 0x0;
        asm volatile("mov x0, %0" :: "r"(tmp));
        asm volatile("str w0, [x1]");  // unmask timer interrupt
    }
}

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

void timer_irq_handler()
{
    timer_list->callback(timer_list->msg);
    timer_list = timer_list->next;  // move head to next
    uint64_t curr_time = time();
    if (timer_list) {
        // still has other tasks
        set_timeout(timer_list->expired_time - curr_time);
    } else {
        // no other tasks, disable timer interrupt
        enable_timer_interrupt(0);
    }

    // volatile uint64_t curr_time;
    // volatile uint64_t freq;
    // asm volatile("mrs %0, cntpct_el0" : "=r"(curr_time));
    // asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));

    // uart_puts("Seconds after booting : ");
    // uart_putx(curr_time / freq);
    // uart_puts("\n");

    // freq <<= 1;
    // asm volatile("msr cntp_tval_el0, %0" :: "r"(freq));
}

void uart_irq_handler()
{
    // disable UART interrupt
    *(uint32_t*)IRQ_DISABLE_IRQS_1 = (1 << IRQ_AUX_INTERRUPT_BIT);


    uint32_t uart_irq_status = *(uint32_t *)AUX_MU_IIR;
    if (uart_irq_status & 0x2) {
        // Transimit holding register empty
        while (tx_buffer.front != tx_buffer.rear){
            uart_putc(front(&tx_buffer));
            pop(&tx_buffer);
        }
        // write done, disable tx interrupt
        *AUX_MU_IER &= ~(0x2);
    } else if (uart_irq_status & 0x4) {
        // Receiver holds valid byte
        while (UART_DATA_READY()) {
            push(&rx_buffer, uart_getc());
        }   
    }

    // enable UART interrupt
    *(uint32_t*)IRQ_ENABLE_IRQS_1 = (1 << IRQ_AUX_INTERRUPT_BIT);
}

void irq_handler()
{
    volatile uint32_t irq_pending_1 = *(uint32_t *)IRQ_IRQ_PENDING_1;
    volatile uint32_t core0_irq_source = *(uint32_t *)CORE0_IRQ_SOURCE;
    if (irq_pending_1 & (1 << IRQ_AUX_INTERRUPT_BIT)) {
        // GPU IRQ 29, AUX Interrupt
        uart_irq_handler();
    } else if (core0_irq_source & (1 << CORE0_CNTPNSIRQ)){
        // Timer Interrupt
        timer_irq_handler();
    }
}

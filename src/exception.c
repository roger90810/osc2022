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
        // set_timeout(timer_list->expired_time - curr_time);
        set_timeout_by_ticks(timer_list->expired_time - curr_time);
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
    thread_schedule();
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

void irq_entry()
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


void syscall_handler(uint64_t syscall_num, struct trapframe *trapframe)
{
    uint64_t ret;
    asm volatile ("msr DAIFClr, 0xf");
    switch (syscall_num) {
        case SYSCALL_GETPID:
            ret = getpid();
            trapframe->regs[0] = ret;
            break;
        case SYSCALL_UART_READ:
            ret = uart_read((char *)trapframe->regs[0], trapframe->regs[1]);
            trapframe->regs[0] = ret;
            break;
        case SYSCALL_UART_WRITE:
            ret = uart_write((char *)trapframe->regs[0], trapframe->regs[1]);
            trapframe->regs[0] = ret;
            break;
        case SYSCALL_EXEC:
            ret = exec(trapframe, (char *)trapframe->regs[0],
                           (char **)trapframe->regs[1]);
            trapframe->regs[0] = ret;
            break;
        case SYSCALL_FORK:
            thread_fork(trapframe);
            break;
        case SYSCALL_EXIT:
            exit();
            break;
        case SYSCALL_MBOX_CALL:
            ret = mbox_call(trapframe->regs[1], (unsigned char)trapframe->regs[0]);
            trapframe->regs[0] = ret;
            break;
        case SYSCALL_KILL:
            kill(trapframe->regs[0]);
            break;
    }
}

int getpid()
{
    return thread_getpid();
}

uint64_t uart_read(char buf[], uint64_t size)
{
    for (uint64_t i = 0; i < size; i++)
        buf[i] = uart_getc();
    return size;
}

uint64_t uart_write(const char buf[], uint64_t size)
{
    for (uint64_t i = 0; i < size; i++)
        uart_putc(buf[i]);
    return size;
}

int exec(struct trapframe *trapframe, char* name, char **argv)
{
    struct thread *curr_thread = get_current_thread();
    void (*func)();
    func = cpio_load(name);
    curr_thread->code_addr = (unsigned long)func;
    trapframe->sp_el0  = (unsigned long)(curr_thread + THREAD_STACK_SIZE);
    trapframe->elr_el1 = (unsigned long)func;
    return 0;
}

void exit()
{
    thread_exit();
    return;
}

void kill(int pid)
{
    thread_kill(pid);
    return;
}

void sync_entry(unsigned long esr, unsigned long elr, struct trapframe *trapframe)
{
    int ec = (esr >> 26) & 0b111111;
    int iss = esr & 0x1FFFFFF;
    if (ec == 0b010101) {  // system call
        uint64_t syscall_num = trapframe->regs[8];
        syscall_handler(syscall_num, trapframe);
    }
    // } else {
    //     uart_puts("Exception return address: ");
    //     uart_putx(elr);
    //     uart_puts("\nException class (EC): ");
    //     uart_putx(ec);
    //     uart_puts("\nInstruction specific syndrome (ISS): ");
    //     uart_putx(iss);
    //     uart_puts("\n");
    // }
}
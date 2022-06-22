#include "thread.h"

struct list_head *idle_queue;
struct list_head *wait_queue;
bool pid_used[MAX_THREAD_NR];

int get_new_pid()
{
    for (int pid = 0; pid < MAX_THREAD_NR; pid++) {
        if (!pid_used[pid]) {
            pid_used[pid] = true;
            return pid;
        }
    }
    return -1;
}

int thread_getpid()
{
    return get_current_thread()->pid;
}

void thread_init()
{
    struct thread *kernel_thread;

    idle_queue = kmalloc(sizeof(struct list_head));
    wait_queue = kmalloc(sizeof(struct list_head));

    INIT_LIST_HEAD(idle_queue);
    INIT_LIST_HEAD(wait_queue);


    for (int i = 0; i < MAX_THREAD_NR; i++) {
        pid_used[i] = false;
    }

    // Create kernel thread
    kernel_thread = kmalloc(sizeof(struct thread));
    kernel_thread->status = THREAD_WAIT;
    kernel_thread->pid    = 0;
    
    for (int i = 0; i < MAX_THREAD_SIG_NR; i++) {
        kernel_thread->signal_handlers[i] = NULL;
        kernel_thread->signal_num[i] = 0;
    }

    list_add_tail(&kernel_thread->list, wait_queue);
    asm volatile("msr tpidr_el1, %0" : : "r"(kernel_thread));

    pid_used[0] = true;

    return;
}

struct thread *thread_create(void (*func)())
{
    struct thread *new_thread = NULL;
    int pid;

    if (idle_queue == NULL) {
        thread_init();
    }

    pid = get_new_pid();

    if (pid != -1) {
        new_thread = kmalloc(sizeof(struct thread));

        new_thread->pid          = pid;
        new_thread->status       = THREAD_IDLE;
        new_thread->code_addr    = (unsigned long)func;
        new_thread->kernel_stack = (uint64_t)kmalloc(PAGE_SIZE << 2);
        new_thread->user_stack   = (uint64_t)kmalloc(PAGE_SIZE << 2);

        new_thread->context.lr = (unsigned long)func;
        new_thread->context.fp = (unsigned long)new_thread->user_stack + THREAD_STACK_SIZE;
        new_thread->context.sp = (unsigned long)new_thread->user_stack + THREAD_STACK_SIZE;

        for (int i = 0; i < MAX_THREAD_SIG_NR; i++) {
            new_thread->signal_handlers[i] = NULL;
            new_thread->signal_num[i] = 0;
        }
    }

    return new_thread;
}

struct thread *get_current_thread()
{
    struct thread *curr_thread;
    asm volatile ("mrs %0, tpidr_el1" : "=r"(curr_thread));
    return curr_thread;
}

void thread_schedule()
{
    struct thread *curr_thread = NULL;
    struct thread *next_thread = NULL;
    curr_thread = get_current_thread();

    check_signal();

    while (1) {
        if (idle_queue->next == idle_queue) {
            return;
        }
        /* has next thread */
        next_thread = (struct thread *)idle_queue->next;
        list_del(idle_queue->next);
        if (next_thread->status == THREAD_IDLE) {
            break;
        }
        if (next_thread->status == THREAD_EXIT) {
            pid_used[next_thread->pid] = false;
            kfree((void *)next_thread->kernel_stack);
            kfree((void *)next_thread->user_stack);
            kfree((void *)next_thread);
        }
    }

    if (next_thread != NULL) {
        // uart_puts("[thread_schedule] curr_thread: ");
        // uart_putx((unsigned long)curr_thread->pid);
        // uart_puts(", next_thread: ");
        // uart_putx((unsigned long)next_thread->pid);
        // uart_puts("\n");
        
        if (curr_thread->pid != 0) {
            list_add_tail(&curr_thread->list, idle_queue);
        }
        switch_to(&curr_thread->context, &next_thread->context, next_thread);
    }
    return;
}

void thread_exit()
{
    struct thread *curr_thread;
    curr_thread = get_current_thread();
    curr_thread->status = THREAD_EXIT;
    thread_schedule();
    return;
}

void thread_kill(int pid)
{
    struct thread *curr_thread;
    asm volatile ("msr DAIFSet, 0xf");
    struct list_head *cur;
    // Find pid thread
    cur = idle_queue->next;
    while (cur != idle_queue) {
        curr_thread = (struct thread *)cur;
        if (curr_thread->pid == pid) {
            uart_puts("Thread pid : ");
            uart_putx(pid);
            uart_puts(" killed\n");
            curr_thread->status = THREAD_EXIT;
            break;
        }
        cur = cur->next;
    }
    cur = wait_queue->next;
    while (cur != wait_queue) {
        curr_thread = (struct thread *)cur;
        if (curr_thread->pid == pid) {
            uart_puts("Thread pid : ");
            uart_putx(pid);
            uart_puts(" killed\n");
            curr_thread->status = THREAD_EXIT;
            break;
        }
        cur = cur->next;
    }
    asm volatile ("msr DAIFClr, 0xf");
    return;
}

void thread_timer_task()
{
    unsigned long cntfrq_el0;
    asm volatile("mrs %0,  cntfrq_el0" : "=r"(cntfrq_el0) : );
    // add_timer(thread_timer_task, NULL, 1);
    add_timer(thread_timer_task, NULL, cntfrq_el0 >> 5);
    return;
}

void thread_exec(void (*func)())
{
    struct thread *new_thread = NULL;
    new_thread = thread_create(func);

    unsigned long tmp;
    asm volatile("mrs %0, cntkctl_el1" : "=r"(tmp));
    tmp |= 1;
    asm volatile("msr cntkctl_el1, %0" : : "r"(tmp));
    add_timer(thread_timer_task, NULL, 5);

    // Print prompt
    uart_puts("User program at: 0x");
    uart_putx((unsigned long) func);
    uart_puts("\n");
    uart_puts("-----------------Entering user program-----------------\n");
   
    // disable UART interrupt
    *(uint32_t*)IRQ_DISABLE_IRQS_1 = (1 << IRQ_AUX_INTERRUPT_BIT);
    *AUX_MU_IER = 0;       // disable interrupt

    asm volatile ("msr tpidr_el1, %0" : : "r"(new_thread));
    from_EL1_to_EL0((unsigned long)func, (unsigned long)new_thread->user_stack + THREAD_STACK_SIZE, (unsigned long)new_thread->kernel_stack + THREAD_STACK_SIZE);
    return;
}

void default_signal_handler()
{
    struct thread *thread = get_current_thread();
    thread->status = THREAD_EXIT;
    return;
}

void *get_signal_handler(struct thread *thread)
{
    void *handler = NULL;
    for (int i = 0; i < MAX_THREAD_SIG_NR; i++) {
        if (thread->signal_num[i] > 0) {
            if (thread->signal_handlers[i] == NULL) {
                // Default signal handler
                handler = default_signal_handler;
            } else {
                handler = thread->signal_handlers[i];
            }
            thread->signal_num[i] -= 1;
            break;
        }
    }
    return handler;
}

void check_signal()
{
    void (*handler) ();
    void *signal_ustack;
    struct thread *thread = get_current_thread();
    unsigned long sp, spsr_el1;    
    while (1) {
        store_context(&thread->signal_save_context);
        handler = get_signal_handler(thread);
        if (handler == NULL) {
            break;
        } else {
            if (handler == default_signal_handler) {
                // Run in kernel mode
                handler();
            } else {
                signal_ustack = kmalloc(PAGE_SIZE << 2);
                // Run in user mode
                asm volatile("msr     elr_el1, %0 \n\t"
                             "msr     sp_el0,  %1 \n\t"
                             "mov     lr, %2       \n\t"
                             "eret" :: "r" (handler), "r"(signal_ustack + THREAD_STACK_SIZE), "r" (syscall_ret));
            }
        }
    }
    return;
}

// Test
void idle_thread_func () {
    while (1) {
        uart_puts("idle thread.\n");
        thread_schedule();
    }
}

void test_thread_func()
{
    for (int i = 0; i < 5; i++) {
        uart_puts("HIHI I'm a thread ^u^, pid: ");
        uart_putx(get_current_thread()->pid);
        uart_puts(", iteration: ");
        uart_putx(i);
        uart_puts("\n");
        thread_schedule();
    }
    thread_exit();
}

void thread_test()
{
    struct thread *thread;
    thread = thread_create(idle_thread_func);
    list_add(&thread->list, idle_queue);
    for (int i = 0; i < 5; i++)
        thread = thread_create(idle_thread_func);
        list_add(&thread->list, idle_queue);
    thread_schedule();
}

void fork_test_func()
{
    uart_puts("\nFork Test, pid ");
    int pid = syscall_getpid();
    uart_putx(pid);
    uart_puts("\n");

    int cnt = 1;
    int ret = 0;

    ret = syscall_fork();

    if (ret == 0) { // child
        long long cur_sp;
        asm volatile("mov %0, sp" : "=r"(cur_sp));

        uart_puts("first child pid: ");
        uart_putx(syscall_getpid());
        uart_puts(", cnt: ");
        uart_putx(cnt);
        uart_puts(", ptr: 0x");
        uart_putx(&cnt);
        uart_puts(", sp : 0x");
        uart_putx(cur_sp);
        uart_puts("\n");

        ++cnt;

        if ((ret = syscall_fork()) != 0){
            asm volatile("mov %0, sp" : "=r"(cur_sp));
            
            uart_puts("first child pid: ");
            uart_putx(syscall_getpid());
            uart_puts(", cnt: ");
            uart_putx(cnt);
            uart_puts(", ptr: 0x");
            uart_putx(&cnt);
            uart_puts(", sp : 0x");
            uart_putx(cur_sp);
            uart_puts("\n");
        }
        else{
            while (cnt < 5) {
                asm volatile("mov %0, sp" : "=r"(cur_sp));

                uart_puts("second child pid: ");
                uart_putx(syscall_getpid());
                uart_puts(", cnt: ");
                uart_putx(cnt);
                uart_puts(", ptr: 0x");
                uart_putx(&cnt);
                uart_puts(", sp : 0x");
                uart_putx(cur_sp);
                uart_puts("\n");

                ++cnt;
            }
        }

        syscall_exit();
    }
    else {
        uart_puts("parent here, pid ");
        uart_putx(syscall_getpid());
        uart_puts(", child ");
        uart_putx(ret);
        uart_puts("\n");
    }
    return;
}

void fork_test()
{
    thread_exec(fork_test_func);
}
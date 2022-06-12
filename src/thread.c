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

void thread_init()
{
    struct thread *kernel_thread;

    idle_queue = kmalloc(sizeof(struct list_head));
    wait_queue = kmalloc(sizeof(struct list_head));

    INIT_LIST_HEAD(idle_queue);
    INIT_LIST_HEAD(wait_queue);

    // thread_queue_init(idle_queue);
    // thread_queue_init(wait_queue);

    for (int i = 0; i < MAX_THREAD_NR; i++) {
        pid_used[i] = false;
    }

    // Create kernel thread
    kernel_thread = kmalloc(sizeof(struct thread));
    kernel_thread->status = THREAD_WAIT;
    kernel_thread->pid    = 0;
    
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

        list_add_tail(&new_thread->list, idle_queue);

        // log_puts("[thread_create] new_thread: 0x", THREAD_LOG_ON);
        // log_puth((unsigned long)new_thread, THREAD_LOG_ON);
        // log_puts("\n", THREAD_LOG_ON);
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
    if (idle_queue->next == idle_queue)
        return;
    struct thread *curr_thread = NULL;
    struct thread *next_thread = NULL;
    curr_thread = get_current_thread();
    while (1) {
        /* has next thread */
        next_thread = (struct thread *)idle_queue->next;
        list_del(idle_queue->next);
        if (next_thread->status == THREAD_IDLE)
            break;

        if (next_thread->status == THREAD_EXIT) {
            pid_used[next_thread->pid] = false;
            kfree((void *)next_thread->kernel_stack);
            kfree((void *)next_thread->user_stack);
            kfree((void *)next_thread);
        }
    }

    // TODO : EXIT should not execute to here???????
    if (next_thread != NULL) {
        // log_puts("[thread_schedule] curr_thread: 0x", THREAD_LOG_ON);
        // log_puth((unsigned long)curr_thread, THREAD_LOG_ON);
        // log_puts(", next_thread: 0x", THREAD_LOG_ON);
        // log_puth((unsigned long)next_thread, THREAD_LOG_ON);
        // log_puts("\n", THREAD_LOG_ON);
        
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
    thread_create(idle_thread_func);
    for (int i = 0; i < 5; i++)
        thread_create(test_thread_func);
    thread_schedule();
}
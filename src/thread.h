#ifndef __THREAD_H
#define __THREAD_H

#include "types.h"
#include "list.h"
#include "uart.h"
#include "mm.h"
#include "object_alloc.h"

#define MAX_THREAD_NR      (32)
#define MAX_THREAD_SIG_NR  (16)
#define THREAD_STACK_SIZE  (PAGE_SIZE << 2)

enum THREAD_STATUS {
    THREAD_IDLE,
    THREAD_RUNNING,
    THREAD_WAIT,
    THREAD_EXIT
};

struct thread_context {
    unsigned long x19;
    unsigned long x20;
    unsigned long x21;
    unsigned long x22;
    unsigned long x23;
    unsigned long x24;
    unsigned long x25;
    unsigned long x26;
    unsigned long x27;
    unsigned long x28;
    unsigned long fp;
    unsigned long lr;
    unsigned long sp;
};

struct thread {
    struct list_head list;
    struct thread_context context;
    int pid;
    unsigned long code_addr;
    unsigned long kernel_stack;
    unsigned long user_stack;
    enum THREAD_STATUS status;
};

struct thread* thread_create(void (*func)());
extern void switch_to(struct thread_context *curr_context,
                      struct thread_context *next_context,
                      struct thread *next_thread);
extern void from_EL1_to_EL0(unsigned long func, unsigned long user_sp, unsigned long kernel_sp);
int get_new_pid();
int thread_getpid();
void thread_init();
struct thread *thread_create(void (*func)());
struct thread *get_current_thread();
void thread_schedule();
void thread_exit();
void thread_exec(void (*func)());
void thread_test();
int thread_fork(struct trapframe* trapframe);
void fork_test();
void thread_kill(int pid);
#endif
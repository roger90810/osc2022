#ifndef __THREAD_H
#define __THREAD_H

#include "types.h"
#include "list.h"
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
void thread_test();

#endif
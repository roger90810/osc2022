#ifndef __SYSCALL_H
#define __SYSCALL_H

#include "types.h"

#define SYSCALL_GETPID          (0)
#define SYSCALL_UART_READ       (1)
#define SYSCALL_UART_WRITE      (2)
#define SYSCALL_EXEC            (3)
#define SYSCALL_FORK            (4)
#define SYSCALL_EXIT            (5)
#define SYSCALL_MBOX_CALL       (6)
#define SYSCALL_KILL            (7)


extern uint64_t syscall_getpid();
extern uint32_t syscall_uart_read(char buf[], uint32_t size);
extern uint32_t syscall_uart_write(const char buf[], uint32_t size);
extern int syscall_exec(void(*func)());
extern int syscall_fork();
extern void syscall_exit();

#endif 
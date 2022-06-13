.global syscall_getpid
syscall_getpid:
    mov x8, 0
    svc 0
    ret

.global syscall_uart_read
syscall_uart_read:
    mov x8, 1
    svc 0
    ret

.global syscall_uart_write
syscall_uart_write:
    mov x8, 2
    svc 0
    ret

.global syscall_exec
syscall_exec:
    mov x8, 3
    svc 0
    ret

.global syscall_fork
syscall_fork:
    mov x8, 4
    svc 0
    ret

.global syscall_exit
syscall_exit:
    mov x8, 5
    svc 0
    ret

.global syscall_mbox_call
syscall_mbox_call:
    mov x8, 6
    svc 0
    ret

.global syscall_kill
syscall_kill:
    mov x8, 7
    svc 0
    ret

.global syscall_ret
syscall_ret:
    mov x8, 10
    svc 0
    ret
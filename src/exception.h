#ifndef __EXCEPTION_H
#define __EXCEPTION_H
#include "types.h"
#include "uart.h"
#include "timer.h"
#include "syscall.h"
#include "thread.h"
#include "mbox.h"
#include "cpio.h"

// Timers interrupt control registers
#define CORE0_TIMER_IRQCNTL 0x40000040 
#define CORE1_TIMER_IRQCNTL 0x40000044 
#define CORE2_TIMER_IRQCNTL 0x40000048 
#define CORE3_TIMER_IRQCNTL 0x4000004C
// Where to route timer interrupt to, IRQ/FIQ
// Setting both the IRQ and FIQ bit gives an FIQ
#define TIMER0_IRQ 0x01
#define TIMER1_IRQ 0x02
#define TIMER2_IRQ 0x04
#define TIMER3_IRQ 0x08
#define TIMER0_FIQ 0x10
#define TIMER1_FIQ 0x20
#define TIMER2_FIQ 0x40
#define TIMER3_FIQ 0x80

// Mailbox interrupt control registers
#define CORE0_MBOX_IRQCNTL 0x40000050
#define CORE1_MBOX_IRQCNTL 0x40000054
#define CORE2_MBOX_IRQCNTL 0x40000058
#define CORE3_MBOX_IRQCNTL 0x4000005C

// Where to route mailbox interrupt to, IRQ/FIQ
// Setting both the IRQ and FIQ bit gives an FIQ
#define MBOX0_IRQ 0x01
#define MBOX1_IRQ 0x02
#define MBOX2_IRQ 0x04
#define MBOX3_IRQ 0x08
#define MBOX0_FIQ 0x10
#define MBOX1_FIQ 0x20
#define MBOX2_FIQ 0x40
#define MBOX3_FIQ 0x80

// IRQ & FIQ source registers
#define CORE0_IRQ_SOURCE 0x40000060
#define CORE1_IRQ_SOURCE 0x40000064
#define CORE2_IRQ_SOURCE 0x40000068
#define CORE3_IRQ_SOURCE 0x4000006C
#define CORE0_FIQ_SOURCE 0x40000070
#define CORE1_FIQ_SOURCE 0x40000074
#define CORE2_FIQ_SOURCE 0x40000078
#define CORE3_FIQ_SOURCE 0x4000007C


// Interrupt source bits
// IRQ and FIQ are the same
// GPU bits can be set for one core only
#define INT_SRC_TIMER0 0x00000001
#define INT_SRC_TIMER1 0x00000002
#define INT_SRC_TIMER2 0x00000004
#define INT_SRC_TIMER3 0x00000008
#define INT_SRC_MBOX0 0x00000010
#define INT_SRC_MBOX1 0x00000020
#define INT_SRC_MBOX2 0x00000040
#define INT_SRC_MBOX3 0x00000080
#define INT_SRC_GPU 0x00000100
#define INT_SRC_PMU 0x00000200

// Mailbox write-set registers (Write only) #define CORE0_MBOX0_SET 0x40000080
#define CORE0_MBOX1_SET 0x40000084
#define CORE0_MBOX2_SET 0x40000088
#define CORE0_MBOX3_SET 0x4000008C
#define CORE1_MBOX0_SET 0x40000090 
#define CORE1_MBOX1_SET 0x40000094 
#define CORE1_MBOX2_SET 0x40000098 
#define CORE1_MBOX3_SET 0x4000009C 
#define CORE2_MBOX0_SET 0x400000A0 
#define CORE2_MBOX1_SET 0x400000A4 
#define CORE2_MBOX2_SET 0x400000A8 
#define CORE2_MBOX3_SET 0x400000AC 
#define CORE3_MBOX0_SET 0x400000B0 
#define CORE3_MBOX1_SET 0x400000B4 
#define CORE3_MBOX2_SET 0x400000B8 
#define CORE3_MBOX3_SET 0x400000BC

// Mailbox write-clear registers (Read & Write) #define CORE0_MBOX0_RDCLR 0x400000C0
#define CORE0_MBOX1_RDCLR 0x400000C4
#define CORE0_MBOX2_RDCLR 0x400000C8
#define CORE0_MBOX3_RDCLR 0x400000CC 
#define CORE1_MBOX0_RDCLR 0x400000D0 
#define CORE1_MBOX1_RDCLR 0x400000D4 
#define CORE1_MBOX2_RDCLR 0x400000D8 
#define CORE1_MBOX3_RDCLR 0x400000DC 
#define CORE2_MBOX0_RDCLR 0x400000E0 
#define CORE2_MBOX1_RDCLR 0x400000E4 
#define CORE2_MBOX2_RDCLR 0x400000E8 
#define CORE2_MBOX3_RDCLR 0x400000EC 
#define CORE3_MBOX0_RDCLR 0x400000F0 
#define CORE3_MBOX1_RDCLR 0x400000F4 
#define CORE3_MBOX2_RDCLR 0x400000F8 
#define CORE3_MBOX3_RDCLR 0x400000FC
#define CORE0_CNTPNSIRQ 1


#define IRQ_BASE                        (0x3F00B000)
#define IRQ_IRQ_BASIC_PENDING           (IRQ_BASE+0x200)
#define IRQ_IRQ_PENDING_1               (IRQ_BASE+0x204)
#define IRQ_IRQ_PENDING_2               (IRQ_BASE+0x208)
#define IRQ_FIR_CONTROL                 (IRQ_BASE+0x20C)
#define IRQ_ENABLE_IRQS_1               (IRQ_BASE+0x210)
#define IRQ_ENABLE_IRQS_2               (IRQ_BASE+0x214)
#define IRQ_ENABLE_BASIC_IRQS           (IRQ_BASE+0x218)
#define IRQ_DISABLE_IRQS_1              (IRQ_BASE+0x21C)
#define IRQ_DISABLE_IRQS_2              (IRQ_BASE+0x220)
#define IRQ_DISABLE_BASIC_IRQS          (IRQ_BASE+0x224)
#define IRQ_AUX_INTERRUPT_BIT           29
#define IRQ_UART_INTERRUPT_BIT          57
#define IRQ_PEND_UART_INTERRUPT_BIT     19

void enable_timer_interrupt(const uint8_t enable);

struct trapframe {
    uint64_t regs[31]; // general register from x0 ~ x30
    uint64_t sp_el0;
    uint64_t elr_el1;
    uint64_t spsr_el1;
};

int getpid ();
uint64_t uart_read (char buf[], uint64_t size);
uint64_t uart_write (const char buf[], uint64_t size);
// int exec (struct trap_frame *trap_frame, char* name, char **argv);
void exit();
void kill (int pid);

#endif
#ifndef __UART_H
#define __UART_H

#include "gpio.h"
#include "stdint.h"
#include "exception.h"
#include "queue.h"
/* Auxilary mini UART registers */

#define AUX_ENABLE      ((volatile unsigned int*)(MMIO_BASE+0x00215004))
#define AUX_MU_IO       ((volatile unsigned int*)(MMIO_BASE+0x00215040))
#define AUX_MU_IER      ((volatile unsigned int*)(MMIO_BASE+0x00215044))
#define AUX_MU_IIR      ((volatile unsigned int*)(MMIO_BASE+0x00215048))
#define AUX_MU_LCR      ((volatile unsigned int*)(MMIO_BASE+0x0021504C))
#define AUX_MU_MCR      ((volatile unsigned int*)(MMIO_BASE+0x00215050))
#define AUX_MU_LSR      ((volatile unsigned int*)(MMIO_BASE+0x00215054))
#define AUX_MU_MSR      ((volatile unsigned int*)(MMIO_BASE+0x00215058))
#define AUX_MU_SCRATCH  ((volatile unsigned int*)(MMIO_BASE+0x0021505C))
#define AUX_MU_CNTL     ((volatile unsigned int*)(MMIO_BASE+0x00215060))
#define AUX_MU_STAT     ((volatile unsigned int*)(MMIO_BASE+0x00215064))
#define AUX_MU_BAUD     ((volatile unsigned int*)(MMIO_BASE+0x00215068))


#define UART_DATA_READY()  ((*AUX_MU_LSR & 0x01))
#define UART_TRANS_EMPTY() ((*AUX_MU_LSR & 0x20))

extern Queue rx_buffer;
extern Queue tx_buffer;

void uart_init();
void uart_putc(const uint32_t c);
char uart_getc();
void uart_puts(const char *s);
void uart_putx(const uint32_t);
void uart_put_hb(const uint8_t c);

#endif
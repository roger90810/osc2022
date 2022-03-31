#include "uart.h"

Queue rx_buffer;
Queue tx_buffer;

void uart_init()
{
    register uint32_t r;
    /* map UART1 to GPIO pins */
    r = *GPFSEL1;
    r &= ~((7 << 12) | (7 << 15)); // clear GPIO14, GPIO15, each GPIO pin has 3-bit
    r |= (2 << 12) | (2 << 15);    // set GPIO14, GPIO15 to ALT5
    *GPFSEL1 = r;
    *GPPUD = 0;            // enable pins 14 and 15
    r = 150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    r = 150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = 0;        // flush GPIO setup

    /* initialize UART */
    *AUX_ENABLE |= 1;      // enable UART1, AUX mini uart
    *AUX_MU_CNTL = 0;      // disable transmitter and receiver
    *AUX_MU_LCR = 3;       // set the data size to 8 bits
    *AUX_MU_MCR = 0;       // don't need auto flow control
    *AUX_MU_IER = 0;       // disable interrupt
    *AUX_MU_IIR = 0xc6;    // no FIFO
    *AUX_MU_BAUD = 270;    // set baud rate to 115200
    *AUX_MU_CNTL = 3;      // enable Tx, Rx
}

void async_uart_init()
{
    rx_buffer.front = -1;
    rx_buffer.rear = -1;
    tx_buffer.front = -1;
    tx_buffer.rear = -1;
    *AUX_MU_IER = 1;       // only enable receive interrupts (bit 0), transmit interrupt (bit 1) should enable when need.
    *(uint32_t*)IRQ_ENABLE_IRQS_1 |= (1 << IRQ_AUX_INTERRUPT_BIT);
}

void async_uart_putc(const char c)
{
    push(&tx_buffer, c);
    *AUX_MU_IER |= 0x2;
}

void async_uart_puts(const char *s)
{
    while (*s) {
        if (*s == '\n') async_uart_putc('\r');
        async_uart_putc(*s++);
    }
}

char async_uart_getc()
{
    do {
        asm volatile("nop");
    } while (rx_buffer.front == rx_buffer.rear);

    char c = front(&rx_buffer);
    pop(&rx_buffer);
    return c;
}

void uart_putc(const uint32_t c)
{
    // Send a character
    /* wait until we can send */
    do {
        asm volatile("nop");
    } while (!UART_TRANS_EMPTY()); // This bit is set if the transmit FIFO can accept at least R one byte.
    /* write the character to the buffer */
    *AUX_MU_IO = c;
}

char uart_getc()
{
    char c;
    /* wait until something is in the buffer */
    do {
        asm volatile("nop");
    } while (!UART_DATA_READY()); // This bit is set if the receive FIFO holds at least 1 symbol.
    /* read it and return */
    c = (char)(*AUX_MU_IO);
    return c;
}

/**
 * Display a string
 */
void uart_puts(const char *s)
{
    while (*s) {
        /* convert newline to carrige return + newline */
        if (*s == '\n')
            uart_putc('\r');
        uart_putc(*s++);
    }
}

void uart_putx(const uint32_t d)
{
    uint32_t n;
    int c;
    for (c = 28; c >= 0; c -= 4) {
        // get highest tetrad
        n = (d >> c) & 0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n += (n > 9)? 0x37 : 0x30;
        uart_putc(n);
    }
}

void uart_put_hb(const uint8_t c)
{
    uint8_t t;
    t = (c >> 4) & 0xf;
    t += (t > 9)? 0x37 : 0x30;
    uart_putc(t);

    t = c & 0xf;
    t += (t > 9)? 0x37 : 0x30;
    uart_putc(t);
}
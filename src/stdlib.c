#include "stdlib.h"

extern unsigned long __heap_start; /* declared in the linker script */
extern unsigned long __heap_size;  /* declared in the linker script */
static unsigned long heap_top = (unsigned long) &__heap_start;
static unsigned long heap_size = (unsigned long) &__heap_size;
static unsigned long *curr_top = (unsigned long *) 0xFFFFFFF0;
static unsigned long *heap_end = (unsigned long *) 0xFFFFFFF0;

void *simple_malloc(unsigned long size)
{
    if (curr_top == heap_end) {
        // first time to call malloc()?
        curr_top = (unsigned long*) heap_top;
        heap_end = (unsigned long*) ((heap_top + heap_size) & 0xFFFFFFF0);
    }
    void *return_ptr = curr_top;
    curr_top = (unsigned long *)((char *)curr_top + size);

    return return_ptr;
}
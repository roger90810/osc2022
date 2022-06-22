#ifndef __MMIO_H
#define __MMIO_H

#include "mm.h"

#define MMIO_PHYSICAL   (0x3F000000)
#define MMIO_BASE       (KERNEL_VIRT_BASE | MMIO_PHYSICAL)

#define PM_PASSWORD (0x5a000000)
#define PM_RSTC     ((volatile unsigned int*) 0x3F10001c)
#define PM_WDOG     ((volatile unsigned int*) 0x3F100024)

void reset(const int tick);
void cancel_reset();

#endif
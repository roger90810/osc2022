#ifndef __SHELL_H
#define __SHELL_H

#include "uart.h"
#include "string.h"
#include "mmio.h"
#include "mbox.h"
#include "cpio.h"
#include "stdlib.h"
#include "dtb.h"
#include "exception.h"

#define CMD_BUF_SIZE 32
#define MAX_ARG_SIZE 4
void start_shell();

#endif
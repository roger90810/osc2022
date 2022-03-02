#include "uart.h"
#include "shell.h"
int main()
{
    uart_init();
    start_shell();
    return 0;
}
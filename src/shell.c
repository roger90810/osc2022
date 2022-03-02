#include "uart.h"
#include "shell.h"
#include "string.h"
#include "mmio.h"
#include "mbox.h"

void echo(const char c)
{
    uart_putc(c);
    if (c == '\n')
        uart_putc('\r');
}

void clear_shell()
{
    uart_putc(0x0c);
}

void cmd_help()
{
    uart_puts("help     : print this help menu\n");
    uart_puts("hello    : print Hello World!\n");
    uart_puts("clear    : clear the screen\n");
    uart_puts("info     : print board information\n");
    uart_puts("reboot   : reboot the device\n");
}

void cmd_hello()
{
    uart_puts("Hello World!\n");
}

void cmd_reboot()
{
    reset(1);
}

void cmd_info()
{
    unsigned int board_revision, base_addr, size;
    get_board_revision(&board_revision);
    uart_puts("Board Revision          : ");
    uart_putx(board_revision);
    uart_puts("\n");

    get_ARM_memory(&base_addr, &size);
    uart_puts("ARM Memory Base Address : ");
    uart_putx(base_addr);
    uart_puts("\n");
    uart_puts("ARM Memory Size         : ");
    uart_putx(size);
    uart_puts("\n");
}

void cmd_invalid()
{
    uart_puts("Invalid command\n");
}

void read_cmd(char *cmd)
{
    char c;
    int cur_pos = 0;
    uart_puts("# ");
    do
    {
        c = uart_getc();
        cmd[cur_pos] = '\0';
        if (c == '\n')
        {
            // read to end
            echo(c);
            break;
        }
        else if (c == '\b')
        {
            // backspace
        }
        else
        {
            echo(c);
            cmd[cur_pos++] = c;
            cmd[cur_pos] = '\0';
        }
    } while (1);
}

void exec_cmd(const char *cmd)
{
    if (strcmp(cmd, "help") == 0)
        cmd_help();
    else if (strcmp(cmd, "hello") == 0)
        cmd_hello();
    else if (strcmp(cmd, "reboot") == 0)
        cmd_reboot();
    else if (strcmp(cmd, "clear") == 0)
        clear_shell();
    else if (strcmp(cmd, "info") == 0)
        cmd_info();
    else
        cmd_invalid();
}

void start_shell()
{
    char cmd[CMD_BUF_SIZE];
    clear_shell();
    while (1)
    {
        // super loop
        read_cmd(cmd);
        exec_cmd(cmd);
    }
}
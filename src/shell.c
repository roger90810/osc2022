#include "shell.h"

extern uint64_t DTB_BASE;

void echo(const char c)
{
    uart_putc(c);
    if (c == '\r')
        uart_putc('\n');
}

void clear_shell()
{
    uart_putc(0x0c);
}

void cmd_help()
{
    uart_puts("help            : print this help menu\n");
    uart_puts("hello           : print Hello World!\n");
    uart_puts("clear           : clear the screen\n");
    uart_puts("info            : print board information\n");
    uart_puts("reboot          : reboot the device\n");
    uart_puts("ls              : print all files\n");
    uart_puts("cat [filename]  : print the content of the file\n");
    uart_puts("malloc          : print the address of memory allocate\n");
    uart_puts("dtb             : print the parse result of dtb\n");
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

void cmd_ls()
{
    cpio_ls();
}

void cmd_cat(const char *file_name)
{
    cpio_cat(file_name);
}

void cmd_malloc()
{
    uart_puts("Simple malloc test:\n");
    char* string = simple_malloc(8);
    for (int i = 0; i < 8; i++) {
        uart_puts("Address of string[");
        uart_putc(i + '0');
        uart_puts("] : ");
        uart_putx(&string[i]);
        uart_puts("\n");
    }
}

void cmd_dtb()
{
    dtb_parser(DTB_BASE, 0);
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
        if (c == '\r') {
            // read to end
            echo(c);
            break;
        } else if (c == '\b') {
            // backspace
        } else {
            echo(c);
            cmd[cur_pos++] = c;
            cmd[cur_pos] = '\0';
        }
    } while (1);
}

void exec_cmd(const char *cmd)
{
    int argc = 0;
    char *argv[MAX_ARG_SIZE];

    // split argv
    for (int i = 0; i < MAX_ARG_SIZE; i++) {;
        char *token = strsep(&cmd, " ");
        if (token == 0) break;
        argv[i] = token;
        argc++;
    }

    if (strcmp(argv[0], "help") == 0)
        cmd_help();
    else if (strcmp(argv[0], "hello") == 0)
        cmd_hello();
    else if (strcmp(argv[0], "reboot") == 0)
        cmd_reboot();
    else if (strcmp(argv[0], "clear") == 0)
        clear_shell();
    else if (strcmp(argv[0], "info") == 0)
        cmd_info();
    else if (strcmp(argv[0], "ls") == 0)
        cmd_ls();
    else if (strcmp(argv[0], "cat") == 0)
        cmd_cat(argv[1]);
    else if (strcmp(argv[0], "malloc") == 0)
        cmd_malloc();
    else if (strcmp(argv[0], "dtb") == 0)
        cmd_dtb();
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
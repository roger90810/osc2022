#include "cpio.h"

/**
 * @brief Convert hex string to unsigned int
 * 
 * @param hex_str input hex string, up to 64-bit
 * @return unsigned int
 */
unsigned long htoi(const char *hex_str)
{
    unsigned long dec_value = 0;
    for (int i = 0; i < 8; i++) {
        dec_value <<= 4;
        if (*hex_str >= '0' && *hex_str <= '9') dec_value += (*hex_str - '0');
        else if (*hex_str >= 'a' && *hex_str <= 'f') dec_value += (10 + (*hex_str - 'a'));
        else if (*hex_str >= 'A' && *hex_str <= 'F') dec_value += (10 + (*hex_str - 'A'));
        else break;
        hex_str++;
    }
    return dec_value;
}

/**
 * @brief Parse CPIO header to get the info of the file.
 * 
 * @param cpio_header Input : New ASCII Format CPIO header
 * @param file_name 
 * @param file_size
 * @param name_size 
 * @return int
 */
int cpio_header_parse(const cpio_newc_header_t *cpio_header, char **file_name, unsigned long *file_size, unsigned long *name_size)
{
    if (strncmp(cpio_header->c_magic, "070701", 6) != 0) return -1;
    *file_size = htoi(cpio_header->c_filesize);
    *name_size = htoi(cpio_header->c_namesize);
    *file_name = (char *)cpio_header + sizeof(cpio_newc_header_t);
    if (strncmp(*file_name, "TRAILER!!!", 10) == 0) return 1;
    if (strncmp(*file_name, "./", 2) == 0) *file_name += 2;
    return 0;
}

void cpio_ls()
{
    cpio_newc_header_t *header = (cpio_newc_header_t *)CPIO_BASE;
    unsigned long file_size;
    unsigned long name_size;
    char *file_name;
    char *next_header_start = (char *)header;
    char t;
    while (cpio_header_parse(header, &file_name, &file_size, &name_size) == 0) {
        uart_puts(file_name);
        uart_puts("\n");
        next_header_start += (sizeof(cpio_newc_header_t) + name_size);
        t = 0x2 - (name_size & 0x3);
        next_header_start += t;

        next_header_start += file_size;
        t = (0x4 - (file_size & 0x7)) & 0x3;
        next_header_start += t;

        header = (cpio_newc_header_t *)next_header_start;
    }
}

void cpio_cat(const char *file_name)
{
    cpio_newc_header_t *header = (cpio_newc_header_t *)CPIO_BASE;
    unsigned long file_size;
    unsigned long name_size;
    char *cpio_file_name;
    char *next_header_start = (char *)header;
    char t;
    char found = 0;
    while (cpio_header_parse(header, &cpio_file_name, &file_size, &name_size) == 0) {
        next_header_start += (sizeof(cpio_newc_header_t) + name_size);
        t = 0x2 - (name_size & 0x3);
        next_header_start += t;

        if (strcmp(file_name, cpio_file_name) == 0) {
            uart_puts(next_header_start);
            uart_puts("\n");
            found = 1;
            break;
        }

        next_header_start += file_size;
        t = (0x4 - (file_size & 0x7)) & 0x3;
        next_header_start += t;

        header = (cpio_newc_header_t *)next_header_start;
    }
    if (!found) {
        uart_puts(file_name);
        uart_puts(": No such file or directory\n");
    }
}
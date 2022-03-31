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
int cpio_header_parse(const cpio_newc_header_t *cpio_header, char **file_name, uint64_t *file_size, uint64_t *name_size)
{
    if (strncmp(cpio_header->c_magic, "070701", 6) != 0) return -1;
    *file_size = htoi(cpio_header->c_filesize);
    *name_size = htoi(cpio_header->c_namesize);
    *file_name = (char *)cpio_header + sizeof(cpio_newc_header_t);
    if (strncmp(*file_name, "TRAILER!!!", 10) == 0) return 1;
    if (strncmp(*file_name, "./", 2) == 0) *file_name += 2;
    return 0;
}

void cpio_traverse(const char *file_name, void (*cpio_callback)(const char*, const char*, const char *, const uint64_t))
{
    cpio_newc_header_t *header = (cpio_newc_header_t *)CPIO_BASE;
    uint64_t file_size;
    uint64_t name_size;
    char *cpio_file_name;
    char *next_header_start = (char *)header;
    while (cpio_header_parse(header, &cpio_file_name, &file_size, &name_size) == 0) {
        file_size = ALIGN(file_size, 4);
        next_header_start += ALIGN((sizeof(cpio_newc_header_t) + name_size), 4);
        cpio_callback(cpio_file_name, next_header_start, file_name, file_size);
        next_header_start += file_size;
        header = (cpio_newc_header_t *)next_header_start;
    }
}

void cpio_callback_ls(const char *cpio_file_name, const char *content, const char *file_name, const uint64_t file_size)
{
    uart_puts(cpio_file_name);
    uart_puts("\n");
}

void cpio_callback_cat(const char *cpio_file_name, const char *content, const char *file_name, const uint64_t file_size)
{
    if (strcmp(cpio_file_name, file_name) != 0) return;
    uart_puts(content);
    uart_puts("\n");
}

void cpio_callback_exec(const char *cpio_file_name, const char *content, const char *file_name, const uint64_t file_size)
{
    if (strcmp(cpio_file_name, file_name) != 0) return;

    volatile uint8_t *base_addr = 0x40000;
    for (int i = 0; i < file_size; i++) {
        base_addr[i] = content[i];
    }
}
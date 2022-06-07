#include "dtb.h"


int dtb_fdt_begin_parse(uint8_t** curr_struct_offset)
{
    if (strncmp(*curr_struct_offset, "\0\0\0\x01", 4) != 0) return -1; // is not FDT_begin token
    *curr_struct_offset += 4;

    // uart_puts(*curr_struct_offset);
    // uart_puts("\n");

    uint32_t name_len;
    name_len = strlen(*curr_struct_offset) + 1;
    *curr_struct_offset += ALIGN(name_len, 4);

    return 0;
}

int dtb_fdt_prop_parse(uint8_t** curr_struct_offset, const uint8_t* string_base, void (*initramfs_callback)(uint8_t*, uint32_t))
{
    if (strncmp(*curr_struct_offset, "\0\0\0\x03", 4) != 0) return -1; // is not FDT_PROP token
    *curr_struct_offset += 4;
    fdt_prop_t *fdt_prop = (fdt_prop_t *)(*curr_struct_offset);
    uint32_t prop_len = fdt_prop->len;
    uint32_t nameoff = fdt_prop->nameoff;
    prop_len = __builtin_bswap32(prop_len);
    nameoff = __builtin_bswap32(nameoff);

    *curr_struct_offset += sizeof(fdt_prop_t);
    const uint8_t *name = string_base + nameoff;
    // print prop and string
    // uart_puts("  - ");
    // uart_puts(name);
    // uart_puts(" : ");

    // for (int i = 0; i < prop_len; i++) {
    //     if ((*curr_struct_offset)[i] == '\0') uart_putc(' ');
    //     else if (IS_PRINTABLE((*curr_struct_offset)[i])) uart_putc((*curr_struct_offset)[i]);
    //     else uart_put_hb((*curr_struct_offset)[i]);
    // }
    // uart_puts("\n");

    if (strcmp(name, "linux,initrd-start") == 0) {
        if (initramfs_callback) (*initramfs_callback)(*curr_struct_offset, prop_len);
    }

    *curr_struct_offset += ALIGN(prop_len, 4);
    return 0;
}

void dtb_parser(uint64_t DTB_BASE, void (*initramfs_callback)(uint8_t*, uint32_t))
{
    fdt_header_t *header = (fdt_header_t *)DTB_BASE;
    uint32_t version = header->version;
    uint32_t off_dt_struct = header->off_dt_struct;
    uint32_t off_dt_strings = header->off_dt_strings;
    version = __builtin_bswap32(version);
    off_dt_struct = __builtin_bswap32(off_dt_struct);
    off_dt_strings = __builtin_bswap32(off_dt_strings);
    uint8_t *curr_struct_offset = (uint8_t *)header + off_dt_struct; // to the begin of struct block
    uint8_t *string_base = (uint8_t *)header + off_dt_strings; // to the begin of string block
    while (dtb_fdt_begin_parse(&curr_struct_offset) == 0) {
        while (dtb_fdt_prop_parse(&curr_struct_offset, string_base, initramfs_callback) == 0) ;
        while (strncmp(curr_struct_offset, "\0\0\0\x02", 4) == 0) curr_struct_offset += 4;
        // uart_puts("\n");
    }
}
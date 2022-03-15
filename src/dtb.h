#ifndef __DTB_H
#define __DTB_H

#include "stdint.h"
#include "uart.h"
#include "string.h"
#include "stdlib.h"

typedef struct {
    uint32_t magic;             // The value 0xd00dfeed (big-endian).
    uint32_t totalsize;         // The total size in bytes of the devicetree data structure.
    uint32_t off_dt_struct;     // The offset in bytes of the structure block from the beginning of the header.
    uint32_t off_dt_strings;    // The offset in bytes of the strings block from the beginning of the header.
    uint32_t off_mem_rsvmap;    // The offset in bytes of the memory reservation block from the beginning of the header.
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;   // The length in bytes of the strings block section of the devicetree blob.
    uint32_t size_dt_struct;    // The length in bytes of the structure block section of the devicetree blob.
} fdt_header_t;

typedef struct {
    uint32_t len;       // The length of the property’s value in bytes
    uint32_t nameoff;   // An offset into the strings block
} fdt_prop_t;

void dtb_parser(uint64_t, void (*initramfs_callback)(uint8_t*, uint32_t));
#endif
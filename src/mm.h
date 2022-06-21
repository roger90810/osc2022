#ifndef __MM_H
#define __MM_H

#include "math.h"
#include "stdlib.h"
#include "string.h"
#include "list.h"
#include "types.h"
#include "uart.h"
#include "object_alloc.h"
#include "dtb.h"

#define PAGE_BASE_ADDR     (0x00000000)
#define PHYSICAL_SIZE      (0x3C000000)
#define PAGE_SHIFT         (12)
#define MAX_ORDER          (11)
#define MAX_ORDER_NR_PAGES (1 << (MAX_ORDER - 1))
#define PAGE_SIZE          (1 << 12)   // 4096 Bytes
#define MAX_NR_PAGES       (PHYSICAL_SIZE >> PAGE_SHIFT)

#ifndef ARCH_PFN_OFFSET
#define ARCH_PFN_OFFSET	   (0UL)
#endif

#define __pfn_to_page(pfn)	(mem_map + ((pfn) - ARCH_PFN_OFFSET))
#define __page_to_pfn(page)	((unsigned long)((page) - mem_map) + \
                            ARCH_PFN_OFFSET)

#define page_to_pfn __page_to_pfn
#define pfn_to_page __pfn_to_page

/*
 * Convert a physical address to a Page Frame Number and back
 */
#define	__phys_to_pfn(paddr) ((unsigned long)((paddr - PAGE_BASE_ADDR) >> PAGE_SHIFT))
#define	__pfn_to_phys(pfn)	 (PAGE_BASE_ADDR + ((unsigned int)(pfn) << PAGE_SHIFT))

#define phys_to_pfn __phys_to_pfn
#define pfn_to_phys __pfn_to_phys

enum PAGE_FLAGS {
    PAGE_BUDDY,
    PAGE_RESERVED
};
struct free_area {
    struct list_head free_list;
    unsigned long nr_free;  // the number of free areas of a given size.
};
struct page {
    struct list_head list;
    int ref_count;
    enum PAGE_FLAGS flags;
    unsigned long order;
};

void mm_init();
struct page *alloc_pages(unsigned int order);
void free_page(unsigned int pfn);

extern struct page *mem_map;
extern struct free_area *free_area;
#endif
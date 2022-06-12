#ifndef __MM_H
#define __MM_H

#include "math.h"
#include "stdlib.h"
#include "string.h"
#include "list.h"
#include "types.h"
#include "uart.h"

#define PAGE_BASE_ADDR     (0x100000)
#define PAGE_SHIFT         (12)
#define MAX_ORDER          (11)
#define MAX_ORDER_NR_PAGES (1 << (MAX_ORDER - 1))
#define PAGE_SIZE          (1 << 12)   // 4096 Bytes
#define MEM_SIZE           (MAX_ORDER_NR_PAGES * PAGE_SIZE) // 1024 * 4KB

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

struct free_area {
    struct list_head free_list;
    unsigned long nr_free;  // the number of free areas of a given size.
};

struct page {
    struct list_head list;
    int ref_count;
    unsigned long flags;
    unsigned long order;
};

void mm_init();
struct page *alloc_pages(unsigned int order);
void free_page(unsigned int pfn);

/*
 *  Only map a fixed, small portion of memory.
 *  0x100000 ~ 0x500000 (4MB)
 *  TODO : parse dtb to get the memory information
 */
extern struct page mem_map[MAX_ORDER_NR_PAGES];
extern struct free_area free_area[MAX_ORDER];

#endif
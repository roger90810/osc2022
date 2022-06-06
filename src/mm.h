#ifndef __MM_H
#define __MM_H
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include "list.h"
#include "types.h"
#include "uart.h"

#define MAX_ORDER   (11)
#define MAX_ORDER_NR_PAGES (1 << (MAX_ORDER - 1))
#define PAGE_SIZE   (1 << 12)   // 4096 Bytes
#define MEM_SIZE    (MAX_ORDER_NR_PAGES * PAGE_SIZE) // 1024 * 4KB

#ifndef ARCH_PFN_OFFSET
#define ARCH_PFN_OFFSET		(0UL)
#endif

#define __pfn_to_page(pfn)	(mem_map + ((pfn) - ARCH_PFN_OFFSET))
#define __page_to_pfn(page)	((unsigned long)((page) - mem_map) + \
				 ARCH_PFN_OFFSET)

#define page_to_pfn __page_to_pfn
#define pfn_to_page __pfn_to_page


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

static void memmap_init();
static void init_free_lists();
void mm_init();
void free_page(unsigned int pfn);

/*
 *  Only map a fixed, small portion of memory.
 *  0x100000 ~ 0x500000 (4MB)
 *  TODO : parse dtb to get the memory information
 */
extern struct page mem_map[MAX_ORDER_NR_PAGES];
extern struct free_area free_area[MAX_ORDER];


#endif
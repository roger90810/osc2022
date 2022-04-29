#ifndef __MM_H
#define __MM_H
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include "list.h"

#define MAX_ORDER   (11)
#define MAX_ORDER_NR_PAGES (1 << (MAX_ORDER - 1))
#define PAGE_SIZE   (1 << 12)   // 4096 Bytes
#define MEM_SIZE    (MAX_ORDER_NR_PAGES * PAGE_SIZE) // 1024 * 4KB

#define PAGE_TYPE_BASE	0xf0000000
#define PAGE_MAPCOUNT_RESERVE   -128
#define PG_buddy        0x00000080
#define PG_offline      0x00000100
#define PG_table        0x00000200
#define PG_guard        0x00000400

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
    unsigned long flags;
    unsigned int page_type;
    unsigned long order;
};

static void memmap_init();
static void init_free_lists();
void mm_init();
#endif
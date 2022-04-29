#include "mm.h"


/*
 *  Only map a fixed, small portion of memory.
 *  0x100000 ~ 0x500000 (4MB)
 *  TODO : parse dtb to get the memory information
 */
struct page mem_map[MAX_ORDER_NR_PAGES];
struct free_area free_area[MAX_ORDER];

void mm_init()
{
    memmap_init();
    init_free_lists();
}

static void init_free_lists()
{
    unsigned int order;
    for (order = 0; order < MAX_ORDER; order++) {
        INIT_LIST_HEAD(&free_area[order].free_list);
        free_area[order].nr_free = 0;
    }
}

static void __init_single_page(struct page *page, unsigned long pfn)
{
    memset(page, 0, sizeof(struct page));
}

static void memmap_init()
{
    unsigned long start_pfn = 0;
    unsigned long end_pfn = MAX_ORDER_NR_PAGES;
    unsigned long hold_pfn = 0;
    unsigned long pfn;
    struct page *page;
    for (pfn = start_pfn; pfn < end_pfn; pfn++) {
        page = pfn_to_page(pfn);
        __init_single_page(page, pfn);
    }
}

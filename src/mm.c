#include "mm.h"


/*
 *  Only map a fixed, small portion of memory.
 *  0x100000 ~ 0x500000 (4MB)
 *  TODO : parse dtb to get the memory information
 */
struct page mem_map[MAX_ORDER_NR_PAGES];
struct free_area free_area[MAX_ORDER];


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
    page->ref_count = 1;
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

/*
 * Locate the struct page for both the matching buddy in our
 * pair (buddy1) and the combined O(n+1) page they form (page).
 *
 * 1) Any buddy B1 will have an order O twin B2 which satisfies
 * the following equation:
 *     B2 = B1 ^ (1 << O)
 * For example, if the starting buddy (buddy2) is #8 its order
 * 1 buddy is #10:
 *     B2 = 8 ^ (1 << 1) = 8 ^ 2 = 10
 *
 * 2) Any buddy B will have an order O+1 parent P which
 * satisfies the following equation:
 *     P = B & ~(1 << O)
 *
 * Assumption: *_mem_map is contiguous at least up to MAX_ORDER
 */
static inline unsigned long
__find_buddy_pfn(unsigned long page_pfn, unsigned int order)
{
        return page_pfn ^ (1 << order);
}

/*
 * This function checks whether a page and its buddy have the same order.
 */
static inline bool page_is_buddy(struct page *page, struct page *buddy,
                                unsigned int order)
{
    if (buddy->order != order)
        return false;
    if (buddy->ref_count != 0)
        return false;
    return true;
}

/* Used for pages not on another list */
static inline void add_to_free_list(struct page *page, unsigned int order)
{
	struct free_area *area = &free_area[order];

	list_add(&page->list, &area->free_list);
	area->nr_free++;
}

/* Used for pages not on another list */
static inline void add_to_free_list_tail(struct page *page, unsigned int order)
{
	struct free_area *area = &free_area[order];

	list_add_tail(&page->list, &area->free_list);
	area->nr_free++;
}

static inline void del_page_from_free_list(struct page *page, unsigned int order)
{
	page->order = 0;
	free_area[order].nr_free--;
}

/*
 * If this is not the largest possible page, check if the buddy
 * of the next-highest order is free. If it is, it's possible
 * that pages are being freed that will coalesce soon. In case,
 * that is happening, add the free page to the tail of the list
 * so it's less likely to be used soon and more likely to be merged
 * as a higher order page
 */
static inline bool
buddy_merge_likely(unsigned long pfn, unsigned long buddy_pfn,
                   struct page *page, unsigned int order)
{
	struct page *higher_page, *higher_buddy;
	unsigned long combined_pfn;

	if (order >= MAX_ORDER - 2)
		return false;

	combined_pfn = buddy_pfn & pfn;
	higher_page = page + (combined_pfn - pfn);
	buddy_pfn = __find_buddy_pfn(combined_pfn, order + 1);
	higher_buddy = higher_page + (buddy_pfn - combined_pfn);

	return page_is_buddy(higher_page, higher_buddy, order + 1);
}

static inline void free_one_page(struct page *page, unsigned long pfn,
                                 unsigned int order)
{
    unsigned int max_order = MAX_ORDER - 1;
    unsigned long buddy_pfn;
    unsigned long combined_pfn;
    struct page *buddy;
    bool to_tail;

continue_merging:
    while (order < max_order) {
        buddy_pfn = __find_buddy_pfn(pfn, order);
        buddy = page + (buddy_pfn - pfn);

        if (!page_is_buddy(page, buddy, order))
            goto done_merging;
            
        del_page_from_free_list(buddy, order);
        combined_pfn = buddy_pfn & pfn;
        page = page + (combined_pfn - pfn);
        pfn = combined_pfn;
        order++;
    }
    if (order < MAX_ORDER - 1) {
        /* If we are here, it means order is >= pageblock_order.
		 * We want to prevent merge between freepages on pageblock
		 * without fallbacks and normal pageblock. Without this,
		 * pageblock isolation could cause incorrect freepage or CMA
		 * accounting or HIGHATOMIC accounting.
		 *
		 * We don't want to hit this code for the more frequent
		 * low-order merging.
		 */
        int buddy_mt;

		buddy_pfn = __find_buddy_pfn(pfn, order);
		buddy = page + (buddy_pfn - pfn);

		if (!page_is_buddy(page, buddy, order))
			goto done_merging;
		max_order = order + 1;
		goto continue_merging;
    }
    
done_merging:
    page->order = order;
    to_tail = buddy_merge_likely(pfn, buddy_pfn, page, order);
	if (to_tail)
		add_to_free_list_tail(page, order);
	else
		add_to_free_list(page, order);
}

void free_pages_core(struct page *page, unsigned int order)
{
    unsigned int nr_pages = 1 << order;
    struct page *p = page;
    for (int i = 0; i < nr_pages; i++) {
        p->ref_count = 0;
        p++;
    }
    free_one_page(page, page_to_pfn(page), order);
}

void mem_init()
{
    int order;
    unsigned long start_pfn = 0;
    unsigned long end_pfn = MAX_ORDER_NR_PAGES;

    while (start_pfn < end_pfn) {

        order = min(MAX_ORDER - 1UL, __ffs(start_pfn));
        while (start_pfn + (1UL << order) > end_pfn)
            order--;
        
        free_pages_core(pfn_to_page(start_pfn), order);
        start_pfn += (1UL << order);
    }
}

static inline struct page *get_page_from_free_area(struct free_area *area)
{
    struct list_head *head = &area->free_list;
	if (head->next == head) {
        // list is empty
        return NULL;
    } else {
        return (struct page *)(head->next);
    }
}

/*
 * The order of subdivision here is critical for the IO subsystem.
 * Please do not alter this order without good reasons and regression
 * testing. Specifically, as large blocks of memory are subdivided,
 * the order in which smaller blocks are delivered depends on the order
 * they're subdivided in this function. This is the primary factor
 * influencing the order in which pages are delivered to the IO
 * subsystem according to empirical testing, and this is also justified
 * by considering the behavior of a buddy system containing a single
 * large block of memory acted on by a series of small allocations.
 * This behavior is a critical factor in sglist merging's success.
 *
 * -- nyc
 */
static inline void expand(struct page *page, int low, int high)
{
	unsigned long size = 1 << high;

	while (high > low) {
		high--;
		size >>= 1;
		add_to_free_list(&page[size], high);
		page[size].order = high;
	}
}

/*
 * Go through the free lists and remove
 * the smallest available page from the freelists
 */
static struct page *
get_page_from_freelists(unsigned int order)
{
    unsigned int current_order;
	struct free_area *area;
	struct page *page;

	/* Find a page of the appropriate size in the preferred list */
	for (current_order = order; current_order < MAX_ORDER; ++current_order) {
		area = &free_area[current_order];
		page = get_page_from_free_area(area);
		if (!page)
			continue;
		del_page_from_free_list(page, current_order);
		expand(page, order, current_order);
        page->ref_count = 1;
		return page;
	}

	return NULL;
}

struct page *alloc_pages(unsigned int order)
{
    struct page *page;
    if (order >= MAX_ORDER) {
        return NULL;
    }
    page = get_page_from_freelists(order);
    return page;
}

void mm_init()
{
    memmap_init();
    init_free_lists();
    mem_init();
    alloc_pages(2);
}
#include "object_alloc.h"

struct pool obj_allocator[MAX_OBJ_ALLOCTOR_NUM];

void pool_init(struct pool* pool, uint64_t size)
{
    pool->obj_size = size;
    pool->obj_per_page = PAGE_SIZE / size;
    pool->page_used = 0;
    pool->obj_used = 0;
    INIT_LIST_HEAD(&pool->free_list);
}

int obj_alloc_register(uint64_t size)
{
    if (size >= PAGE_SIZE) {
        uart_puts("Object too large\n");
        return -1;
    }

    // normalize size
    uint64_t size_per_allocator = PAGE_SIZE / MAX_OBJ_ALLOCTOR_NUM;
    uint64_t normalize_block = size / size_per_allocator;
    if (size % size_per_allocator) {
        normalize_block++;
    }
    int normalize_size = normalize_block * size_per_allocator;

    for (int i = 0; i < MAX_OBJ_ALLOCTOR_NUM; i++) {
        if (obj_allocator[i].used && obj_allocator[i].obj_size == normalize_size) {
            return i;
        } else if (!obj_allocator[i].used) {
            obj_allocator[i].used = true;
            pool_init(&obj_allocator[i], normalize_size);
            return i;
        }
    }

    uart_puts("No avaliable pool for current request\n");
    return -1;
}

void* obj_alloc_kernel(int token) 
{
    int pool_num = token;
    struct pool* pool = &obj_allocator[pool_num];

    uint64_t addr;

    // reused free obj
    struct list_head *head = &pool->free_list;
    if (head->next != head) {
        addr = (uint64_t)head->next;
        list_del(head->next);
        return (void *)addr;
    }

    // need new page
    if (pool->obj_used >= pool->page_used * pool->obj_per_page) {
        struct page *page = alloc_pages(0);
        unsigned long pfn = page_to_pfn(page);
        pool->page_addr[pool->page_used] = pfn_to_phys(pfn);
        pool->page_used++;
    }

    // allocate new obj
    addr = pool->page_addr[pool->page_used - 1] + pool->obj_used * pool->obj_size;
    pool->obj_used++;
    return (void*) addr;
}

void obj_free(int token, void* addr)
{
    int pool_num = token;
    struct pool* pool = &obj_allocator[pool_num];
    list_add_tail((struct list_head*)addr, &pool->free_list);
    pool->obj_used--;
}

void* kmalloc(uint64_t size)
{
    // size too large
    if (size >= PAGE_SIZE) {
        // uart_puts("kmalloc using buddy\n");
        int order;
        for (int i = 0; i < MAX_ORDER; i++) {
            if (size <= (uint64_t)((1 << i) * PAGE_SIZE)) {
                order = i;
                break;
            }
        }
        struct page *page = alloc_pages(order);
        unsigned long pfn = page_to_pfn(page);
        return (void*)pfn_to_phys(pfn);
    }
    else {
        // uart_puts("kmalloc using object allocator\n");
        // check exist object allocator
        for (int i = 0; i < MAX_OBJ_ALLOCTOR_NUM; i++) {
            if ((uint64_t)obj_allocator[i].obj_size == size) {
                return (void*)obj_alloc_kernel(i);
            }
        }
        // register new obj allocator
        int token = obj_alloc_register(size);
        return obj_alloc_kernel(token);
    }
}

void kfree(void* p)
{
    int addr_pfn;
    int page_pfn;
    for (int i = 0; i < MAX_OBJ_ALLOCTOR_NUM; i++) {
        for (int j = 0; j < obj_allocator[i].page_used; j++) {
            addr_pfn = phys_to_pfn((uint64_t)p);
            page_pfn = phys_to_pfn(obj_allocator[i].page_addr[j]);
            if (addr_pfn == page_pfn) {
                // uart_puts("free using obj allocator\n");
                obj_free(i, p);
                return;
            }
        }
    }
    // uart_puts("free using buddy\n");
    free_page(addr_pfn);
}
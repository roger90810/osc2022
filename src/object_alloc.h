#ifndef __OBJECT_ALLOC_H
#define __OBJECT_ALLOC_H

#include "types.h"
#include "mm.h"
#include "uart.h"

#define MAX_POOL_PAGES        16
#define MAX_OBJ_ALLOCTOR_NUM  32

struct pool {
    bool used;
    int obj_size;
    int obj_per_page;
    int obj_used;
    int page_used;
    uint64_t page_addr[MAX_POOL_PAGES];
    struct list_head free_list;
};

void* kmalloc(uint64_t size);
void kfree(void* p);

#endif
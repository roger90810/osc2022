#ifndef __TIMER_H
#define __TIMER_H

#include "types.h"
#include "stdlib.h"
#include "string.h"
#include "exception.h"
#include "list.h"
#include "object_alloc.h"

typedef struct timer_t
{
    struct list_head list; 
    uint32_t expired_time;
    char* msg;
    void(*callback)();
} timer_t;
extern struct list_head* timer_list;
void set_timeout(const uint32_t sec);
void set_timeout_by_ticks(const uint32_t ticks);
void add_timer(void(*callback)(), const char* msg, const uint32_t after);
uint64_t time();
void timer_init();

#endif
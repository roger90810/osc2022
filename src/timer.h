#ifndef __TIMER_H
#define __TIMER_H
#include "types.h"
#include "stdlib.h"
#include "string.h"

typedef struct timer_t
{
    uint32_t expired_time;
    char* msg;
    struct timer_t* next;
    void(*callback)();
} timer_t;
extern timer_t* timer_list;
void set_timeout(const uint32_t sec);
void add_timer(void(*callback)(), const char* msg, const uint32_t after);
uint64_t time();
#endif
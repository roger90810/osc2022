#include "timer.h"

timer_t* timer_list;
void add_timer(void(*callback)(), const char* msg, const uint32_t after)
{
    uint64_t curr_time = time();
    timer_t* new_timer = simple_malloc(sizeof(timer_t));
    new_timer->msg = simple_malloc(sizeof(msg) + 1);
    strncpy(new_timer->msg, msg, strlen(msg));
    new_timer->expired_time = curr_time + after;
    new_timer->callback = callback;
    new_timer->next = 0;

    timer_t* cur = timer_list;
    timer_t* prev = 0;

    if (!timer_list) {
        timer_list = new_timer;
        // set_timeout(after);
        set_timeout_by_ticks(after);
        enable_timer_interrupt(1);
    } else {
        // find the position
        while (cur) {
            if (new_timer->expired_time < cur->expired_time) {
                break;
            }
            prev = cur;
            cur = cur->next;
        }
        
        if (!prev) {
            // insert to head
            new_timer->next = cur;
            timer_list = new_timer;
            set_timeout_by_ticks(after);
        } else {
            new_timer->next = cur;
            prev->next = new_timer;
        }
    }
}

void set_timeout(const uint32_t sec)
{
    volatile uint64_t freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    freq *= sec;
    asm volatile("msr cntp_tval_el0, %0" :: "r"(freq));
}

void set_timeout_by_ticks(const uint32_t ticks)
{
    unsigned long cntpct_el0;
    unsigned long cntp_cval_el0;
    asm volatile("mrs %0,  cntpct_el0" : "=r"(cntpct_el0) : );
    cntp_cval_el0 = cntpct_el0 + ticks;
    // Set next expire time
    asm volatile ("msr cntp_cval_el0, %0" :: "r"(cntp_cval_el0));
}

uint64_t time()
{
    volatile uint64_t curr_cnt;
    volatile uint64_t freq;
    asm volatile("mrs %0, cntpct_el0" : "=r"(curr_cnt));
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    return (curr_cnt / freq); // curr time in sec = curr counter / frequncy
}
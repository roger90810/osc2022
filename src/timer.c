#include "timer.h"

struct list_head *timer_list;

void timer_init()
{
    timer_list = kmalloc(sizeof(struct list_head));
    INIT_LIST_HEAD(timer_list);
}

void add_timer(void(*callback)(), const char* msg, const uint32_t after)
{
    uint64_t curr_time = time();
    timer_t* new_timer = kmalloc(sizeof(timer_t));
    uint64_t len = strlen(msg);
    new_timer->msg = kmalloc(len + 1);
    strncpy(new_timer->msg, msg, len);
    new_timer->msg[len] = '\0';
    new_timer->expired_time = curr_time + after;
    new_timer->callback = callback;
    INIT_LIST_HEAD(&new_timer->list);

    struct list_head *cur = timer_list->next;
    if (timer_list->next == timer_list) {
        list_add(&new_timer->list, timer_list);
        // set_timeout(after);
        set_timeout_by_ticks(after);
        enable_timer_interrupt(1);
    } else {
        // find the position
        while (cur != timer_list) {
            timer_t *cur_timer = (timer_t *)cur;
            if (new_timer->expired_time < cur_timer->expired_time) {
                break;
            }
            cur = cur->next;
        }
        list_add_tail(&new_timer->list, cur);
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
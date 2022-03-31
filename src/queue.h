#ifndef __QUEUE_H
#define __QUEUE_H
#include "stdint.h"

#define MAX_SIZE 1024

typedef struct {
    uint32_t front;
    uint32_t rear;
    char arr[MAX_SIZE];
} Queue;

void push(Queue*, const char);
void pop(Queue*);
char front(Queue*);

#endif
#include "queue.h"

void push(Queue *q, const char c)
{
    // if (q->rear == MAX_SIZE - 1) return // buf overflow 

    // use circular queue
    q->rear = (q->rear + 1) % MAX_SIZE;
    if (q->rear == q->front) return;
    q->arr[q->rear] = c;
}

void pop(Queue *q)
{
    // use circular queue
    if (q->front == q->rear) return; // Queue is empty
    q->front = (q->front + 1) % MAX_SIZE;
}

char front(Queue *q)
{
    if (q->front == q->rear) return '\0'; // Queue is empty
    return q->arr[q->front + 1];
}
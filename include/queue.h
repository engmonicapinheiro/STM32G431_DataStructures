#ifndef __QUEUE_H__
#define __QUEUE_H__

struct Queue
{
    int front;
    int rear;
    int capacity;
    int size;
    int *array;
};

typedef struct Queue queue_t;

queue_t * QueueCreate(int capacity);
void QueueEnqueue(queue_t *queue, int data);
int QueueDequeue(queue_t *queue);
int QueueGetSize(queue_t *queue);
int QueueGetFrontElement(queue_t *queue);
int QueueGetRearElement(queue_t *queue);
int QueueIsEmpty(queue_t *queue);
int QueueIsFull(queue_t *queue);
void QueueDelete(queue_t *queue);


#endif //QUEUE_H
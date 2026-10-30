#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

queue_t * QueueCreate(int capacity)
{
    queue_t *queue = (queue_t *) malloc(sizeof(queue_t));

    if (queue == NULL)
    {
        printf("Memory error\r\n");
        return NULL;
    }

    queue->capacity = capacity;
    queue->front = -1;
    queue->rear = -1;
    queue->size = 0;

    queue->array = (int *)malloc(queue->capacity * sizeof(int));
    if (!queue->array)
    {
        printf("Memory error\r\n");
        return NULL;
    }

    return queue;
}

void QueueEnqueue(queue_t *queue, int data)
{
    if (QueueIsFull(queue))
    {
        printf("Queue overflow\r\n");
        return;
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = data;

    if (queue->front == -1)
    {
        queue->front = queue->rear;
    }
    queue->size++;
}

int QueueDequeue(queue_t *queue)
{
    int data = -999;
    if (QueueIsEmpty(queue))
    {
        printf("Queue underflow\r\n");
        return -1;
    }

    data = queue->array[queue->front];

    if (queue->front == queue->rear)
    {
        queue->front = queue->rear = -1;
        queue->size = 0;
    }
    else
    {
        /* computing the index */
        queue->front = (queue->front + 1) % queue->capacity;
        queue->size--;
    }

    return data;
}

int QueueGetSize(queue_t *queue)
{
    return queue->size;
}

int QueueGetFrontElement(queue_t *queue)
{
    return queue->array[queue->front];
}

int QueueGetRearElement(queue_t *queue)
{
    return queue->array[queue->rear];
}

int QueueIsEmpty(queue_t *queue)
{
    return (queue->size == 0);
}

int QueueIsFull(queue_t *queue)
{
    return (queue->size == queue->capacity);
}

void QueueDelete(queue_t *queue)
{
    if (queue)
    {
        if (queue->array)
        {
            free(queue->array);
        }
        free(queue);
    }
}
#ifndef __PRIORITY_QUEUE_TASK_EXECUTOR_HPP__
#define __PRIORITY_QUEUE_TASK_EXECUTOR_HPP__

#include <stdint.h>

#define MAX_TASKS  10

/* task function pointer */
typedef void (*TaskFunction)(void);

/* task structure */
typedef struct
{
    uint8_t priority; //lower value means higher priority
    TaskFunction task;
} Task_t;

static Task_t taskQueue[MAX_TASKS];


void AddTask(TaskFunction task, uint8_t priority);
void ExecuteTask(void);

/* example tasks */
void TaskLedToggle(void);
void TaskReadAdc(void);
void TaskSendData(void);

/* task profiler */
typedef uint32_t taskProfiler;




#endif //PRIORITY_QUEUE_TASK_EXECUTOR
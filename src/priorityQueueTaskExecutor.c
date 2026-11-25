#include "priorityQueueTaskExecutor.h"
#include "stm32g431xx.h"
#include "gpio.h"
#include "adc.h"
#include <stdio.h>

static uint8_t taskCount = 0;
taskProfiler task1Profiler, task2Profiler, task3Profiler;

void AddTask(TaskFunction task, uint8_t priority)
{
    if (taskCount >= MAX_TASKS)
    {
        printf("Task queue is full.\r\n");
        return;
    }

    /* insert the task into the queue based on priority */
    uint8_t i = taskCount;

    while (i > 0 && taskQueue[i-1].priority > priority)
    {
        taskQueue[i] = taskQueue[i-1];
        i--;
    }
    taskQueue[i].task = task;
    taskQueue[i].priority = priority;
    taskCount++;
}


void ExecuteTask(void)
{
    while (taskCount > 0)
    {
        TaskFunction task = taskQueue[0].task;
        /* shift tasks in the queue */
        for (uint8_t i = 1; i < taskCount; i++)
        {
            taskQueue[i - 1] = taskQueue[i];
        }
        taskCount--;

        /* execute the task */
        task();
    }

}

void TaskLedToggle(void)
{
    LedToggle();
    printf("Toggling LED\r\n");
    task1Profiler++;

}
void TaskReadAdc(void)
{
   // uint32_t adcValue = AdcRead();
    //printf("ADC value %lu\r\n", adcValue);
    printf("ADC value...\r\n");
    task2Profiler++;

}
void TaskSendData(void)
{
    printf("Sending data over UART\r\n");
    task3Profiler++;
}

/* temporary IRQHandler */
void USART2_IRQHandler(void)
{
    /* RXNEIE also enables the overrun interrupt. ORE is NOT cleared by
 * reading RDR, so if it is left set the IRQ re-enters forever and
 * main() never runs again. Clear it explicitly. */
    if (USART2->ISR & USART_ISR_ORE)
    {
        USART2->ICR = USART_ICR_ORECF;
    }

    if (USART2->ISR & USART_ISR_RXNE)
    {
        /* reading RDR also clears RXNE */
        uint8_t receivedData = (uint8_t)(USART2->RDR & 0xFF);
    }
}
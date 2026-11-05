#ifndef __CIRCULAR_QUEUE_H__
#define __CIRCULAR_QUEUE_H__

#include <stdint.h>
#include <stdbool.h>

#define QUEUE_SIZE  32
#define ADC_BUFFER_SIZE  64

/* declare enum for possible commands to perform */
typedef enum
{
    COMMAND_LED_ON = 1,
    COMMAND_LED_OFF,
    COMMAND_READ_ADC
} CommandType_t;

/* structure for a single command */
typedef struct
{
    CommandType_t commandType;
    uint32_t time;
} Command_t;


typedef struct
{
    uint8_t buffer[QUEUE_SIZE]; //fixed-size buffer for queue storage
    volatile uint8_t head;  //index of the first element
    volatile uint8_t tail;  //index of the next insertion point
} UART_circularQueue_t;

typedef struct
{
    uint8_t buffer[ADC_BUFFER_SIZE]; //fixed-size buffer for adc samples
    volatile uint8_t head;
    volatile uint8_t tail;
} ADC_circularQueue_t;

typedef struct
{
    Command_t buffer[QUEUE_SIZE]; //fixed-size buffer for adc samples
    volatile uint8_t head;
    volatile uint8_t tail;
} Command_circularQueue_t;


bool IsUartCircularQueueFull(void);
bool IsADCCircularQueueFull(void);
bool IsCommandCircularQueueFull(void);

bool IsUartCircularQueueEmpty(void);
bool IsADCCircularQueueEmpty(void);
bool IsCommandCircularQueueEmpty(void);

bool EnqueueCircularADCQueue(uint32_t data);
bool EnqueueCircularUARTQueue(uint32_t data);
bool EnqueueCircularCommandQueue(Command_t command);

bool DequeueADCCircularQueue(uint32_t *data);
bool DequeueUartCircularQueue(uint8_t *data);
bool DequeueCommandCircularQueue(Command_t *command);

void ProcessUartData(void);
void ProcessCircularQueueCommands(void);
void CollectADCData(void);

#endif //CIRCULAR_QUEUE_H
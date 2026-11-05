#include "circularQueue.h"
#include <stdio.h>
#include <stdint.h>
#include "adc.h"
#include "gpio.h"
#include "stm32g431xx.h"

/* circular queue instance */
static UART_circularQueue_t uartRxQueue = {
    .head = 0,
    .tail = 0
};

/* circular queue instance for the ADC */
static ADC_circularQueue_t ADC_dataQueue = {
    .head = 0,
    .tail = 0
};

/* circular queue instance for the commands */
static Command_circularQueue_t Command_dataQueue = {
    .head = 0,
    .tail = 0
};

bool IsUartCircularQueueFull(void)
{
    return (((uartRxQueue.tail + 1) % QUEUE_SIZE) == uartRxQueue.head);
}

bool IsADCCircularQueueFull(void)
{
    return (((ADC_dataQueue.tail + 1) % ADC_BUFFER_SIZE) == ADC_dataQueue.head);
}

bool IsCommandCircularQueueFull(void)
{
    return (((Command_dataQueue.tail + 1) % QUEUE_SIZE) == Command_dataQueue.head);
}


bool IsUartCircularQueueEmpty(void)
{
    return (uartRxQueue.tail == uartRxQueue.head);
}

bool IsADCCircularQueueEmpty(void)
{
    return (ADC_dataQueue.tail == ADC_dataQueue.head);
}

bool IsCommandCircularQueueEmpty(void)
{
    return (Command_dataQueue.tail == Command_dataQueue.head);
}

bool EnqueueCircularADCQueue(uint32_t data)
{
    if (IsADCCircularQueueFull())
    {
        return false;
    }
    ADC_dataQueue.buffer[ADC_dataQueue.tail] = data;  //store data into buffer
    ADC_dataQueue.tail = (ADC_dataQueue.tail + 1) % ADC_BUFFER_SIZE;  //move tail to the next position

    return true;
}

bool EnqueueCircularUARTQueue(uint32_t data)
{
    if (IsUartCircularQueueFull())
    {
        return false;
    }
    uartRxQueue.buffer[uartRxQueue.tail] = data;  //store data into buffer
    uartRxQueue.tail = (uartRxQueue.tail + 1) % QUEUE_SIZE;  //move tail to the next position

    return true;
}

bool EnqueueCircularCommandQueue(Command_t command)
{
    if (IsCommandCircularQueueFull())
    {
        return false;
    }
    Command_dataQueue.buffer[Command_dataQueue.tail] = command;  //store data into buffer
    Command_dataQueue.tail = (Command_dataQueue.tail + 1) % QUEUE_SIZE;  //move tail to the next position

    return true;
}


bool DequeueADCCircularQueue(uint32_t *data)
{
    if (IsADCCircularQueueEmpty())
    {
        return false;
    }
    *data = ADC_dataQueue.buffer[ADC_dataQueue.head];  //retrieve data
    ADC_dataQueue.head = (ADC_dataQueue.head + 1) % ADC_BUFFER_SIZE;  //move front to the next element

    return true;
}

bool DequeueUartCircularQueue(uint8_t *data)
{
    if (IsUartCircularQueueEmpty())
    {
        return false;
    }
    *data = uartRxQueue.buffer[uartRxQueue.head];  //retrieve data
    uartRxQueue.head = (uartRxQueue.head + 1) % QUEUE_SIZE;  //move front to the next element

    return true;
}

bool DequeueCommandCircularQueue(Command_t *command)
{
    if (IsCommandCircularQueueEmpty())
    {
        return false;
    }
    *command = Command_dataQueue.buffer[Command_dataQueue.head];  //retrieve data
    Command_dataQueue.head = (Command_dataQueue.head + 1) % QUEUE_SIZE;  //move front to the next element

    return true;
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
        EnqueueCircularUARTQueue(receivedData);

    }
}

void ProcessUartData(void)
{
    uint8_t receivedByte;
    Command_t command;

    while (DequeueUartCircularQueue(&receivedByte))
    {
        switch (receivedByte)
        {
        case '1':
            command.commandType = COMMAND_LED_ON;
            EnqueueCircularCommandQueue(command);
            break;
        case '2':
            command.commandType = COMMAND_LED_OFF;
            EnqueueCircularCommandQueue(command);
            break;
        case '3':
            command.commandType = COMMAND_READ_ADC;
            EnqueueCircularCommandQueue(command);
            break;
        default:
            break;
        }
    }
}

void ProcessCircularQueueCommands(void)
{
    Command_t command;
    uint32_t adcValue;
    while (DequeueCommandCircularQueue(&command))
    {
        switch (command.commandType)
        {
            case COMMAND_LED_ON:
            LedOn();
            printf("Led turned on\r\n");
            break;

        case COMMAND_LED_OFF:
            LedOff();
            printf("Led turned off\r\n");
            break;

        case COMMAND_READ_ADC:
            while (DequeueADCCircularQueue(&adcValue))
            {
                printf("ADC value: %lu\r\n", adcValue);
            }
            break;
        }
    }
}

void CollectADCData(void)
{
    uint32_t adcValue = AdcRead();
    EnqueueCircularADCQueue(adcValue);
}
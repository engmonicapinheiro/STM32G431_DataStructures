#include "stm32g431xx.h"
#include <stdio.h>
#include <stdint.h>
#include "fpu.h"
#include "uart.h"
#include "timebase.h"
#include "gpio.h"
#include "adc.h"
#include "commands.h"

static LinkedList_t commandQueue;

int main()
{
    /* enable the FPU */
    FpuEnable();
    /* initialise the UART */
    UartInit();
    /* initialise timebase */
    TimebaseInit();
    /* initialise the LED */
    LedInit();
    /* initialise the ADC */
     AdcInit();
    /* start conversion */
    StartConversion();

    /* initialise the command queue */
    commandQueue.head = NULL;

    while (1)
    {
      //  printf("Hello again from STM32G431xx...\n\r");

        /* process the commands */
        ProcessCommands(&commandQueue);
        delay(100);

    }
}


uint8_t receivedData;

void USART2_IRQHandler(void)
{
    if (USART2->CR1 & USART_CR1_RXNEIE)
    {
        //check if data is received
        receivedData = USART2->RDR;  //read received byte
        Command_t command;  //create a new command struct

        switch (receivedData)
        {
            case '1':
                command.commandType = COMMAND_LED_ON;
                command.data = 0;
                InsertAtTail(&commandQueue, command);
                break;

            case '2':
            command.commandType = COMMAND_LED_OFF;
            command.data = 0;
            InsertAtTail(&commandQueue, command);
            break;

            case '3':
            command.commandType = COMMAND_READ_ADC;
            command.data = AdcRead();
            InsertAtTail(&commandQueue, command);
            break;

            default:
                break;
        }
    }
}
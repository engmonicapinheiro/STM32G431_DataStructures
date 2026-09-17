#include "stm32g431xx.h"
#include <stdio.h>
#include <stdint.h>
#include "fpu.h"
#include "uart.h"
#include "timebase.h"
#include "gpio.h"
#include "adc.h"
#include "commands.h"

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
    InitialiseCommandQueue();

    printf("Hello again from STM32G431xx...\n\r");

    while (1)
    {
        /* process the commands */
        CallProcessCommands();
    }
}

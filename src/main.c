#include "stm32g431xx.h"
#include <stdio.h>
#include <stdbool.h>
#include "fpu.h"
#include "uart.h"
#include "timebase.h"
#include "gpio.h"


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


    while (1)
    {
        //printf("Hello from STM32G431xx...\n\r");
       // delay(1);
        LedOn();
        delay(2);
        LedOff();
        delay(2);
    }

}

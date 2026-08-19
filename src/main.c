#include "stm32g431xx.h"
#include <stdio.h>
#include "fpu.h"
#include "uart.h"
#include "timebase.h"

int main()
{
    /* enable the FPU */
    FpuEnable();
    /* initialise the UART */
    UartInit();
    /* initialise timebase */
    TimebaseInit();

    while (1)
    {
        printf("Hello from STM32G431...\n\r");
        delay(1);
    }

}

#include "stm32g431xx.h"
#include <stdio.h>
#include "fpu.h"
#include "uart.h"


int main()
{
    /* enable the FPU */
    FpuEnable();
    /* initialise the UART */
    UartInit();

    while (1)
    {
        printf("Hello from STM32G431...\n\r");
        for (int i = 0; i < 9000; ++i){}
    }

}

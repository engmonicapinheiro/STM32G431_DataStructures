#include "timebase.h"
#include "stm32g431xx.h"


volatile uint32_t currentTick;
volatile uint32_t currentTick_previous;



void TimebaseInit(void)
{
    /* disable global interrupts */
    __disable_irq();
    /* load the timer with number of clock cycles per second */
   // SysTick->LOAD = ONE_SECOND_LOAD - 1;
    SysTick->LOAD = ONE_MSECOND_LOAD - 1;
    /* clear the systick current value register */
    SysTick->VAL = 0;
    /* select internal clock source */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk;
    /* enable interrupts */
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    /* enable systick */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    /* enable global interrupts */
    __enable_irq();

}

/* delay in seconds */
void delay(uint32_t delayinterval)
{
    uint32_t tickStart = GetTick();
    uint32_t wait = delayinterval;

    if (wait < MAX_DELAY)
    {
        wait += (uint32_t)TICK_FREQUENCY;
    }

    while ((GetTick() - tickStart) < wait){}

}


uint32_t GetTick(void)
{
    __disable_irq();
    currentTick_previous = currentTick;
    __enable_irq();
    return currentTick_previous;
}

void TickIncrement(void)
{
    currentTick += TICK_FREQUENCY;
}

void SysTick_Handler(void)
{
    TickIncrement();
}
#ifndef __TIMEBASE_H__
#define __TIMEBASE_H__

#include <stdint.h>

#define ONE_SECOND_LOAD     16000000
#define ONE_MSECOND_LOAD    16000
#define TICK_FREQUENCY      1
#define MAX_DELAY           0xFFFFFFFF

void TimebaseInit(void);
void TickIncrement(void);
uint32_t GetTick(void);
void delay(uint32_t delayinterval);

#endif //TIMEBASE_H
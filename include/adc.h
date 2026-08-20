#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>

void AdcInit(void);
void StartConversion(void);
uint32_t AdcRead(void);

#endif //ADC_H
#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>
#include <stdbool.h>

/* STM32G431 factory temperature calibration values: 12-bit ADC, VREF+ = 3.0 V. */
#define TEMP_CAL_30C   (*(const volatile uint16_t *)0x1FFF75A8UL)
#define TEMP_CAL_130C  (*(const volatile uint16_t *)0x1FFF75CAUL)


void AdcInit(void);
void StartConversion(void);
uint32_t AdcRead(void);
void InternalTemperatureSensorInit(void);
bool ReadInternalTemperatureCelcius(uint32_t vref_plus_mv, float *temperature_c);


#endif //ADC_H
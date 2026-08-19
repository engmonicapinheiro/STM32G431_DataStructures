#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdint.h>
#include <stdbool.h>

void LedInit(void);
void LedOn(void);
void LedOff(void);
void ButtonInit(void);
bool GetButtonState(void);

#endif //GPIO_H
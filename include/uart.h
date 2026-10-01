#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>

#define UART_BAUDRATE_DEBUG  115200
#define SYS_FREQUENCY (16000000U)
#define APB1_CLOCK    (SYS_FREQUENCY)
#define UART_RX_BUFFER_SIZE   (32U)

void UartInit(void);
int _write(int fd, char *ptr, int len);
int UartReadInt(void);
int UartReadCharNonBlocking(uint8_t *ch);
uint8_t UartReadRingBuffer(void);

#endif //UART_H
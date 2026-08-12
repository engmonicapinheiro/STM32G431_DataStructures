#ifndef __UART_H__
#define __UART_H__

#define UART_BAUDRATE_DEBUG  115200
#define SYS_FREQUENCY (16000000)
#define APB1_CLOCK    (SYS_FREQUENCY)

void UartInit(void);
int _write(int fd, char *ptr, int len);

#endif //UART_H
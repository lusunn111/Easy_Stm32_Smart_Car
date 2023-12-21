#ifndef __UART_H
#define __UART_H

void UART_Init(void);
void ProcessData(uint8_t data[]);
void UART_STOP(void);
void UART_START(void);
#endif

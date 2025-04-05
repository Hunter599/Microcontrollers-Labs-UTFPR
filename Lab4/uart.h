#include <stdint.h>

void UART_Init(void);
unsigned char UART_Receive(void);
void UART_Transmit(unsigned char character);
void UART_SendString(unsigned char* string);

uint8_t receive_from_uart(char* data);
void write_uart(char* data);
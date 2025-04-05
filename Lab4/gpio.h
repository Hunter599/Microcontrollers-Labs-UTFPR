#include <stdint.h>

void GPIO_Init(void);
uint32_t PortJ_Input(void);
void PortN_Output(uint32_t leds);
void PortF_Output(uint32_t leds);
void GPIOPortJ_Handler(void);
void Timer1A_Handler(void);
void Timer2A_Handler(void);

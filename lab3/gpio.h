
#include <stdint.h>

void GPIO_Init(void);
uint32_t PortJ_Input(void); 
void PortN_Output(uint32_t data);
void PortA_Output(uint32_t data);
void GPIOPortJ_Handler(void);
void Timer2A_Handler(void); 
void PortK_Output(uint32_t data); 
uint32_t PortL_Input(void);
void PortM_Output(uint32_t data);
void PortP_Output(uint32_t data); 
void PortQ_Output(uint32_t data);
void PortH_Output(uint32_t phases);


void zerar_memoria(void);
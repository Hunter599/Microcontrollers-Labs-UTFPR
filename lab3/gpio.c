// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Inicializa as portas J e N
// Prof. Guilherme Peron


#include <stdint.h>
#include "tm4c1294ncpdt.h" // Device header file (register definitions)

#include "gpio.h"


void SysTick_Wait1ms(uint32_t delay);

// Declaring the external functions
extern void zerar_memoria(void);
extern void EnableInterrupts(void);
extern void print_status();

int stepperMotorActive = 0;
extern uint32_t motor_State; // 0 Full, 1 Half


// -------------------------------------------------------------------------------
// Função GPIO_Init
// Parâmetro de entrada: Não tem
// Parâmetro de saída: Não tem

// Define ports
#define GPIO_PORTA  0x0001  // SYSCTL_PPGPIO_P0
#define GPIO_PORTJ  0x0100  // SYSCTL_PPGPIO_P8
#define GPIO_PORTK  0x0200  // SYSCTL_PPGPIO_P9
#define GPIO_PORTL  0x0400  // SYSCTL_PPGPIO_P10
#define GPIO_PORTM  0x0800  // SYSCTL_PPGPIO_P11
#define GPIO_PORTP  0x2000  // SYSCTL_PPGPIO_P13
#define GPIO_PORTQ  0x4000  // SYSCTL_PPGPIO_P14
#define GPIO_PORTH 0x00000080	// SYSCTL_PPGPIO_P7



#define GPIO_PORTJ_RIS_R    (*((volatile uint32_t *)0x40060414))

void BlinkLED(void);


void GPIO_Init(void) {
    // 1. Enable clock for all GPIO ports and wait for them to be ready
    SYSCTL_RCGCGPIO_R |= GPIO_PORTA | GPIO_PORTJ | GPIO_PORTH | GPIO_PORTK | GPIO_PORTL | GPIO_PORTM | GPIO_PORTP | GPIO_PORTQ;
    
		
		while ((SYSCTL_PRGPIO_R & (GPIO_PORTA | GPIO_PORTJ | GPIO_PORTH | GPIO_PORTK | GPIO_PORTL | GPIO_PORTM | GPIO_PORTP | GPIO_PORTQ)) == 0);

    // 2. Disable analog function by clearing AMSEL for all ports
    GPIO_PORTA_AHB_AMSEL_R = 0x00;
    GPIO_PORTJ_AHB_AMSEL_R = 0x00;
    GPIO_PORTK_AMSEL_R = 0x00;
    GPIO_PORTL_AMSEL_R = 0x00;
    GPIO_PORTM_AMSEL_R = 0x00;
    GPIO_PORTP_AMSEL_R = 0x00;
    GPIO_PORTQ_AMSEL_R = 0x00;
		GPIO_PORTH_AHB_AMSEL_R = 0x00;

    // 3. Configure all pins as GPIO (clear PCTL registers)
    GPIO_PORTA_AHB_PCTL_R = 0x00000000;
    GPIO_PORTJ_AHB_PCTL_R = 0x00000000;
    GPIO_PORTK_PCTL_R = 0x00;
    GPIO_PORTL_PCTL_R = 0x00;
    GPIO_PORTM_PCTL_R = 0x00;
    GPIO_PORTP_PCTL_R = 0x00;
    GPIO_PORTQ_PCTL_R = 0x00;
		GPIO_PORTH_AHB_PCTL_R = 0x00;

    // 4. Set direction (DIR): 0 = input, 1 = output
    GPIO_PORTA_AHB_DIR_R = 0xF0; // PA7-PA4 output
    GPIO_PORTJ_AHB_DIR_R = 0x00; // PJ0 output
    GPIO_PORTK_DIR_R     = 0xFF; // PK7-PK0 output
    GPIO_PORTL_DIR_R     = 0x00; // PL3-PL0 input
    GPIO_PORTM_DIR_R     = 0xF7; // PM7-PM4 and PM2-PM0 output
    GPIO_PORTP_DIR_R     = 0x20; // PP5 output
    GPIO_PORTQ_DIR_R     = 0x0F; // PQ3-PQ0 output
		GPIO_PORTH_AHB_DIR_R = 0x0F;				// 1111			: PH3 ao PH0

    // 5. Disable alternate functions (AFSEL) for all ports
    GPIO_PORTA_AHB_AFSEL_R = 0x00;
    GPIO_PORTJ_AHB_AFSEL_R = 0x00;
    GPIO_PORTK_AFSEL_R     = 0x00;
    GPIO_PORTL_AFSEL_R     = 0x00;
    GPIO_PORTM_AFSEL_R     = 0x00;
    GPIO_PORTP_AFSEL_R     = 0x00;
    GPIO_PORTQ_AFSEL_R     = 0x00;
		GPIO_PORTH_AHB_AFSEL_R = 0x00;

    // 6. Enable digital functionality (DEN) for specific pins
    GPIO_PORTA_AHB_DEN_R |= 0xF0; // PA7-PA4
    GPIO_PORTJ_AHB_DEN_R |= 0x03; // PJ1-PJ0
    GPIO_PORTK_DEN_R     |= 0xFF; // PK7-PK0
    GPIO_PORTL_DEN_R     |= 0x0F; // PL3-PL0
    GPIO_PORTM_DEN_R     |= 0xF7; // PM7-PM4 and PM2-PM0
    GPIO_PORTP_DEN_R     |= 0x20; // PP5
    GPIO_PORTQ_DEN_R     |= 0x0F; // PQ3-PQ0
		GPIO_PORTH_AHB_DEN_R = 0x0F;				// 1111			: PH3 ao PH0

    // 7. Enable internal pull-up resistors for PJ and PL pins
    GPIO_PORTJ_AHB_PUR_R = 0x03; // PJ1-PJ0 pull-up resistors
    GPIO_PORTL_PUR_R     = 0x0F; // PL3-PL0 pull-up resistors

    // 8. Configure interrupt for PJ pins (PJ0 and PJ1)
    GPIO_PORTJ_AHB_IM_R  = 0x00;   // Disable interrupts on PJ1-PJ0
    GPIO_PORTJ_AHB_IS_R  = 0x00;   // Edge-sensitive interrupts
    GPIO_PORTJ_AHB_IBE_R = 0x00;   // Single-edge interrupts
    GPIO_PORTJ_AHB_IEV_R = 0x00;   // Falling-edge interrupts
    GPIO_PORTJ_AHB_ICR_R = 0x03;    // Clear interrupt flags on PJ1-PJ0
    GPIO_PORTJ_AHB_IM_R  = 0x03;    // Enable interrupts on PJ1-PJ0

    NVIC_EN1_R = 0x80000;         // Enable IRQ for Port J in NVIC
		NVIC_PRI12_R = 0xA0000000;		// Configures priority
		
		EnableInterrupts();
		
				
				// 1. Enable Timer 2
		SYSCTL_RCGCTIMER_R = 0x04;  // Enable clock for Timer 2

		// Wait for Timer 2 module to be ready
		while ((SYSCTL_PRTIMER_R & 0x04) == 0) {
				// Busy wait until Timer 2 is ready
		}

		// 2. Disable Timer 2 before configuration
		TIMER2_CTL_R &= 0xFFE;  // Clear bit 0 to disable Timer 2

		// 3. Configure Timer 2 as a 32-bit timer
		TIMER2_CFG_R &= 0x00;  // Clear bits [2:0] to select 32-bit configuration

		// 4. Set Timer 2 mode to periodic
		TIMER2_TAMR_R = 0x02;   // Set bits [1:0] to 0b10 for periodic mode

		// 5. Set Timer 2 interval load value
		TIMER2_TAILR_R = 0xC34FF;  // Load value for the timer

		// 6. Set Timer 2 prescaler
		TIMER2_TAPR_R = 0x00;  // No prescaler

		// 7. Clear Timer 2 timeout flag
		TIMER2_ICR_R |= 0x01;  // Write 1 to clear the timeout flag

		// 8. Enable Timer 2 timeout interrupt
		TIMER2_IMR_R |= 0x01;  // Set bit 0 to enable timeout interrupt

		// Set Timer 2 interrupt priority to 4 (on bits 29-31 of NVIC_PRI5_R)
		NVIC_PRI5_R = 0x20000000;

		// Enable Timer 2 interrupt in NVIC (bit 23 of NVIC_EN0_R)
		NVIC_EN0_R = 0x800000;

		// 9. Enable Timer 2
		TIMER2_CTL_R |= 0x01;  // Set bit 0 to enable Timer 2

		
}



// -------------------------------------------------------------------------------
// Função PortJ_Input
// Lê os valores de entrada do port J
// Parâmetro de entrada: Não tem
// Parâmetro de saída: o valor da leitura do port
uint32_t PortJ_Input(void) 
{
    return GPIO_PORTJ_AHB_DATA_R; // Read the data register
}

// -------------------------------------------------------------------------------
// Função PortN_Output
// Escreve os valores no port N
// Parâmetro de entrada: Valor a ser escrito
// Parâmetro de saída: não tem
void PortN_Output(uint32_t data)
{
    uint32_t temp;
    //vamos zerar somente os bits menos significativos
    //para uma escrita amigável nos bits 0 e 1
    temp = GPIO_PORTN_DATA_R & 0xFC;
    //agora vamos fazer o OR com o valor recebido na função
    temp = temp | data;
    GPIO_PORTN_DATA_R = temp; 
}

// Function: PortA_Output
void PortA_Output(uint32_t data) {
    uint32_t current_value;
    current_value = GPIO_PORTA_AHB_DATA_R;
    current_value &= ~0xF0; // Mask to clear PA7:PA4
    GPIO_PORTA_AHB_DATA_R = (data | current_value); // Set the new value
}





// Function: Timer2A_Handler
void Timer2A_Handler(void) {
		
    TIMER2_ICR_R = 0x01; // Clear the interrupt
    
		if (stepperMotorActive == 0)
			stepperMotorActive = 1;
		
    // Re-enable the timer
    TIMER2_CTL_R = 0x01;
}

#include "lcd.h"
// Function: GPIOPortJ_Handler
void GPIOPortJ_Handler(void) {
		
    uint32_t status = GPIO_PORTJ_RIS_R;
		
		//Se a USR_SW2 estiver pressionada
		if (PortJ_Input() == 0x1)
			zerar_memoria();
    //Se a USR_SW1 estiver pressionada
		else if (PortJ_Input() == 0x2)
			if (motor_State == 0)
				{
					motor_State = 1; // 0 Full, 1 Half
				}
			else
				{
					motor_State = 0; // 0 Full, 1 Half
				}
			
		print_status();
		
    // Clear the interrupt (acknowledge)
    GPIO_PORTJ_AHB_ICR_R = 0x03;
		return;
		
}



// Function: PortK_Output
void PortK_Output(uint32_t data) {
    uint32_t current_value;
    current_value = GPIO_PORTK_DATA_R;
    current_value &= ~0xFF; // Mask to clear PK7:PK0
    GPIO_PORTK_DATA_R = (data | current_value); // Set the new value
}

// Function: PortL_Input
uint32_t PortL_Input(void) {
    return GPIO_PORTL_DATA_R; // Read the data register
}

// Function: PortM_Output
void PortM_Output(uint32_t data) {
    uint32_t current_value;
    current_value = GPIO_PORTM_DATA_R;
    current_value &= ~0xF7; // Mask to clear PM7:PM4 and PM3:PM0
    GPIO_PORTM_DATA_R = (data | current_value); // Set the new value
}

// Function: PortP_Output
void PortP_Output(uint32_t data) {
    uint32_t current_value;
    current_value = GPIO_PORTP_DATA_R;
    current_value &= ~0x20; // Mask to clear PP5
    GPIO_PORTP_DATA_R = (data | current_value); // Set the new value
}

// Function: PortQ_Output
void PortQ_Output(uint32_t data) {
    uint32_t current_value;
    current_value = GPIO_PORTQ_DATA_R;
    current_value &= ~0xF; // Mask to clear PQ3:PQ0
    GPIO_PORTQ_DATA_R = (data | current_value); // Set the new value

}

// Funcao PortH_Output
// Escreve no Port H
// Parametro de entrada: Quais fases do motor deve acionar
// Parametro de saida: N?o tem
void PortH_Output(uint32_t phases)
{
	uint32_t temp;
	temp = GPIO_PORTH_AHB_DATA_R & 0x00;
	
	temp = temp | phases;
	
	GPIO_PORTH_AHB_DATA_R = temp;
}


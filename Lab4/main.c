#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tm4c1294ncpdt.h"
#include "gpio.h"


#define INITIAL_STATE 0x0
#define POT_RUN_STATE 0x2
#define TERM_RUN_STATE 0x3

void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);



void led_bar_on_off(uint8_t status);
void led_bar_write(uint8_t value);

void enable_motor(uint8_t);
void set_motor_direction(uint8_t);

uint32_t read_adc(void);

void init_uart(void);
void write_uart(char* data);
uint8_t receive_from_uart(char* data);
void handle_uart_receive(void);

uint8_t state = INITIAL_STATE;
uint8_t motor_direction = 0x1;
uint8_t motor_pwm_status = 0x1;
uint8_t duty_cycle = 0;
uint32_t pwm_max_ticks = 80000;
uint32_t pot_value = 0;

int main(void)
{
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	init_uart();
	enable_motor(0x1);
	write_uart("Motor parado\n\r");
	write_uart("\n\rPressione '*' para continuar...\n\r");
	char data = 0;
	while (1)
	{
			handle_uart_receive();
			
			if (state == POT_RUN_STATE){
				pot_value = read_adc();
			int32_t dc = duty_cycle - 100*pot_value/4096.f;
				
			if (dc < 0) dc*=-1;
			if (dc > 1){
				duty_cycle = 100*(pot_value)/4096.f;
			
				
				
				if (pot_value >= 2048){
					motor_direction = 0x2;
				}
				else{
					motor_direction = 0x1;
				}
				
				char str[50];
				sprintf(str, "Velocidade do motor: %d\%\n\r", duty_cycle);
				write_uart(str);
			}
				}
				
				enable_motor(duty_cycle);
				set_motor_direction(motor_direction);
			}
		
		}
		
	


void handle_uart_receive(void) {
    char data = 0;
    while (receive_from_uart(&data)) {
        switch (data) {
            case '*':
                // Step 2: Prompt for control mode selection
                write_uart("\n\rSelecione o modo de controle:\n\r");
                write_uart("  p: Controle por potenciometro\n\r");
                write_uart("  t: Controle por terminal\n\r");
                state = INITIAL_STATE;  // Reset state when '*' is pressed
                break;

            case 'p':
                if (state == INITIAL_STATE) {
                    state = POT_RUN_STATE;
                    write_uart("\n\rControle de velocidade por potenciometro selecionado.\n\r");
                }
                break;

            case 't':
                if (state == INITIAL_STATE) {
                    state = TERM_RUN_STATE;
                    write_uart("\n\rControle de velocidade por terminal selecionado.\n\r");
                    write_uart("Selecione o sentido de rotacao:\n\r");
                    write_uart("  h: Horario (clockwise)\n\r");
                    write_uart("  a: Anti-horario (counterclockwise)\n\r");
                }
                break;

            case 'h':
                if (state == TERM_RUN_STATE) {
                    write_uart("\n\rSentido de rotacao HORARIO selecionado.\n\r");
                    motor_direction = 0x1;
                    write_uart("Selecione a velocidade [0-6]:\n\r");
                }
                break;

            case 'a':
                if (state == TERM_RUN_STATE) {
                    write_uart("\n\rSentido de rotacao ANTI-HORARIO selecionado.\n\r");
                    motor_direction = 0x2;
                    write_uart("Selecione a velocidade [0-6]:\n\r");
                }
                break;

            case '5': case '6': case '7': case '8': case '9':
                if (state == TERM_RUN_STATE) {
                    duty_cycle = (data - '0') * 10;  // Convert ASCII to int (50-100%)
                    char str[50];
                    sprintf(str, "Velocidade do motor: %d%%\n\r", duty_cycle);
                    write_uart(str);
									
                }
                break;
						case '0':  // Ensure '0' corresponds to 100% speed
						if (state == TERM_RUN_STATE) {
								duty_cycle = 100;  // Set full speed
								write_uart("Velocidade do motor: 100%\n\r");
						}
						break;

            case 's':  // Stop motor and return to menu
						if (state == TERM_RUN_STATE || state == POT_RUN_STATE) {
								write_uart("\n\rMotor Parado\n\r");
								duty_cycle = 0;  // Stop motor
								enable_motor(duty_cycle);  // Apply 0% PWM
								state = INITIAL_STATE;  // Reset state
								write_uart("\n\rPressione '*' para continuar...\n\r");
						}
						break;


            default:
                break;
        }
    }
}


uint32_t read_adc(void){
	ADC0_PSSI_R = 0x0008;
	while ((ADC0_RIS_R & 0x8u) != 8u) {};
	uint32_t data = ADC0_SSFIFO3_R;
	ADC0_ISC_R = ADC0_ISC_R & ~0x8u;
	return data;
}

void init_uart(void){
	SYSCTL_RCGCUART_R = SYSCTL_RCGCUART_R0;
	while ((SYSCTL_RCGCUART_R & SYSCTL_RCGCUART_R0) != SYSCTL_RCGCUART_R0) {};
	UART0_CTL_R = UART0_CTL_R & ~UART_CTL_UARTEN; // Garantimos que a uart esteja desligada
	UART0_IBRD_R = 520; // baud-rate parte inteira
	UART0_FBRD_R = 53; // baud-rate parte fracionaria
	UART0_LCRH_R = UART0_LCRH_R  | UART_LCRH_WLEN_8;
	UART0_LCRH_R = UART0_LCRH_R | UART_LCRH_FEN;
	UART0_LCRH_R = UART0_LCRH_R & ~UART_LCRH_STP2;
	UART0_LCRH_R = UART0_LCRH_R & ~UART_LCRH_EPS;
	UART0_LCRH_R = UART0_LCRH_R & ~UART_LCRH_PEN;
  UART0_CC_R = UART_CC_CS_SYSCLK;
  UART0_CTL_R = UART0_CTL_R | UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN;
	UART0_CTL_R = UART0_CTL_R & ~UART_CTL_HSE;	
}

void write_uart(char* data){
		for (int i = 0; data[i] != '\0'; i++){
			while ((UART0_FR_R & UART_FR_TXFF) == UART_FR_TXFF) {};
			UART0_DR_R = data[i];
		}
	}


uint8_t receive_from_uart(char* data){
	uint8_t status = 0;

	if ((UART0_FR_R & UART_FR_RXFE) != UART_FR_RXFE) {
		*data = UART0_DR_R & UART_DR_DATA_M;
		status = 1;
	}

	return status;
}

void enable_motor(uint8_t status){
	//if (motor_pwm_status == 0x1){
	if (status >= 0){
		GPIO_PORTF_AHB_DATA_R = 0x8u;
		TIMER1_CTL_R = TIMER1_CTL_R | (0x1u);
	} else {
		GPIO_PORTF_AHB_DATA_R = ~0x8u;
		TIMER1_CTL_R = TIMER1_CTL_R | (~0x1u);
	}
}

void set_motor_direction(uint8_t direction){
	if (motor_pwm_status == 0x0){
		GPIO_PORTE_AHB_DATA_R = (GPIO_PORTE_AHB_DATA_R & ~0x3u);
		GPIO_PORTN_DATA_R = (GPIO_PORTN_DATA_R & ~0x3u) | 0x3u;
	} else {
		GPIO_PORTE_AHB_DATA_R = (GPIO_PORTE_AHB_DATA_R & ~0x3u) | (direction & 0x3u);
		GPIO_PORTN_DATA_R = (GPIO_PORTN_DATA_R & ~0x3u) | (direction & 0x3u);
	}
}

void led_bar_on_off(uint8_t status){
	GPIO_PORTP_DATA_R = status ? GPIO_PORTP_DATA_R | 32u : GPIO_PORTP_DATA_R & ~(32u);
}

void led_bar_write(uint8_t value){
	GPIO_PORTA_AHB_DATA_R = (GPIO_PORTA_AHB_DATA_R & ~(0xF0u)) | (value & (0xF0));
	GPIO_PORTQ_DATA_R = (GPIO_PORTQ_DATA_R & ~(0x0Fu)) | (value & (0x0F));
}


void Timer1A_Handler(void){
	TIMER1_ICR_R |= 0x01;
	if (motor_pwm_status == 0x0){
		motor_pwm_status = 0x1;
		TIMER1_TAILR_R = pwm_max_ticks * duty_cycle/100;
	} else {
		TIMER1_TAILR_R = pwm_max_ticks * (100 - duty_cycle)/100;
		motor_pwm_status = 0x0;
	}
	
}


// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Verifica o estado das chaves USR_SW1 e USR_SW2, acende os LEDs 1 e 2 caso estejam pressionadas independentemente
// Caso as duas chaves estejam pressionadas ao mesmo tempo pisca os LEDs alternadamente a cada 500ms.
// Prof. Guilherme Peron

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tm4c1294ncpdt.h"


#define INITIAL_STATE 0x0
#define POT_RUN_STATE 0x2
#define TERM_RUN_STATE 0x3

void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);

void GPIO_Init(void);
uint32_t PortJ_Input(void);
void PortN_Output(uint32_t leds);
void PortF_Output(uint32_t leds);
void GPIOPortJ_Handler(void);
void Timer1A_Handler(void);
void Timer2A_Handler(void);
void lcd_port_m(uint32_t);
void lcd_data_write(char);

void update_led(void);
void update_lcd(void);
void led_bar_on_off(uint8_t status);
void led_bar_write(uint8_t value);

void enable_motor(uint8_t);
void set_motor_direction(uint8_t);

uint32_t read_adc(void);

void init_uart(void);
void write_2_uart(char* data, uint32_t size);
uint8_t receive_from_uart(char* data);
void handle_uart_receive(void);

uint8_t state = INITIAL_STATE;
uint8_t motor_direction = 0x1;
uint8_t motor_pwm_status = 0x1;
uint8_t duty_cycle = 0;
uint32_t pwm_max_ticks = 100000;
uint32_t pot_value = 0;

int main(void)
{
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	init_uart();
	enable_motor(0x1);
	write_2_uart("MOTOR PARADO\n\r", 0);
	write_2_uart("Selecione o sentido de rotacao (a: anti-horario | h:horario)\n\r", 0);
	while (1)
	{
		handle_uart_receive();
		if (state == POT_RUN_STATE){
			pot_value = read_adc();
			int32_t dc = duty_cycle - 100*pot_value/4096.f;
			if (dc < 0) dc*=-1;
			if (dc > 1){
				duty_cycle = 100*pot_value/4096.f;
				char str[50];
				sprintf(str, "Velocidade do motor: %d\%\n\r", duty_cycle);
				write_2_uart(str,0);
			}
		}
		
		enable_motor(duty_cycle);
		set_motor_direction(motor_direction);
	}
}

void handle_uart_receive(void){
	char data = 0;
	while(receive_from_uart(&data)){
		switch(data){
			case 'a':
				write_2_uart("Sentido de rotacao ANTI-HORARIO\n\r", 0);
				motor_direction = 0x2;
				if (state == INITIAL_STATE)
					write_2_uart("Selecione o modo de controle (p: potenciometro | t:terminal)\n\r", 0);
				break;
			case 'h':
				write_2_uart("Sentido de rotacao HORARIO\n\r", 0);
				if (state == INITIAL_STATE)
					write_2_uart("Selecione o modo de controle (p: potenciometro | t:terminal)\n\r", 0);
				motor_direction = 0x1;
				break;
			case 't':
				if (state == INITIAL_STATE){
					state = TERM_RUN_STATE;
					write_2_uart("Controle de velocidade por terminal\n\r", 0);
					write_2_uart("Selecione a velocidade [0-6]\n\r", 0);
				}
				break;
			case 'p':
				if (state == INITIAL_STATE){
					state = POT_RUN_STATE;
					write_2_uart("Controle de velocidade por potenciometro\n\r", 0);
				}
				break;
			case '0':
				if (state == TERM_RUN_STATE){
					write_2_uart("MOTOR PARADO\n\r", 0);
					duty_cycle = 0;
				}
				break;
			case '1':
				if (state == TERM_RUN_STATE){
					write_2_uart("Velocidade do motor: 50%\n\r", 0);
					duty_cycle = 50;
				}
				break;
			case '2':
				if (state == TERM_RUN_STATE){
					write_2_uart("Velocidade do motor: 60%\n\r", 0);
					duty_cycle = 60;
				}
				break;
			case '3':
				if (state == TERM_RUN_STATE){
					write_2_uart("Velocidade do motor: 70%\n\r", 0);
					duty_cycle = 70;
				}
				break;
			case '4':
				if (state == TERM_RUN_STATE){
					write_2_uart("Velocidade do motor: 80%\n\r", 0);
					duty_cycle = 80;
				}
				break;
			case '5':
				if (state == TERM_RUN_STATE){
					write_2_uart("Velocidade do motor: 90%\n\r", 0);
					duty_cycle = 90;
				}
				break;
			case '6':
				if (state == TERM_RUN_STATE){
					write_2_uart("Velocidade do motor: 100%\n\r", 0);
					duty_cycle = 99;
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

void write_2_uart(char* data, uint32_t size){
	if (size > 0){
		for (int i = 0; i < size; i++){
			while ((UART0_FR_R & UART_FR_TXFF) == UART_FR_TXFF) {};
			UART0_DR_R = data[i];
		}
	} else {
		for (int i = 0; data[i] != '\0'; i++){
			while ((UART0_FR_R & UART_FR_TXFF) == UART_FR_TXFF) {};
			UART0_DR_R = data[i];
		}
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

void Timer2A_Handler(void){
	TIMER2_ICR_R |= 0x01;

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


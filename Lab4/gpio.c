// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Inicializa as portas J e N
// Prof. Guilherme Peron


#include <stdint.h>

#include "tm4c1294ncpdt.h"

  
#define GPIO_PORTA  (0x1)
#define GPIO_PORTB  (0x2)
#define GPIO_PORTE  (0x10)
#define GPIO_PORTH  (128) 
#define GPIO_PORTJ  (0x100) //bit 8
#define GPIO_PORTK  (0x200)
#define GPIO_PORTL  (0x400)
#define GPIO_PORTM  (0x800)
#define GPIO_PORTN  (0x1000) //bit 12
#define GPIO_PORTF  (0x20) //bit 5
#define GPIO_PORTP  (8192) 
#define GPIO_PORTQ  (16384) 

// -------------------------------------------------------------------------------
// Função GPIO_Init
// Inicializa os ports J e N
// Parâmetro de entrada: Não tem
// Parâmetro de saída: Não tem
void GPIO_Init(void)
{
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R = (GPIO_PORTA | GPIO_PORTE | GPIO_PORTH | GPIO_PORTJ | GPIO_PORTK | GPIO_PORTL | GPIO_PORTM | GPIO_PORTN | GPIO_PORTF | GPIO_PORTP | GPIO_PORTQ);
	//1b.   após isso verificar no PRGPIO se a porta está pronta para uso.
  while((SYSCTL_PRGPIO_R & (GPIO_PORTA | GPIO_PORTE | GPIO_PORTH | GPIO_PORTJ | GPIO_PORTK | GPIO_PORTL | GPIO_PORTM | GPIO_PORTN | GPIO_PORTF | GPIO_PORTP | GPIO_PORTQ) ) != (GPIO_PORTA | GPIO_PORTE | GPIO_PORTH | GPIO_PORTJ | GPIO_PORTK | GPIO_PORTL | GPIO_PORTM | GPIO_PORTN | GPIO_PORTF | GPIO_PORTP | GPIO_PORTQ) ){};
	
	// 2. Limpar o AMSEL para desabilitar a analógica
	GPIO_PORTA_AHB_AMSEL_R = 0x00;
	GPIO_PORTE_AHB_AMSEL_R = 0x10;
	GPIO_PORTH_AHB_AMSEL_R = 0x00;
	GPIO_PORTJ_AHB_AMSEL_R = 0x00;
	GPIO_PORTK_AMSEL_R = 0x00;
	GPIO_PORTL_AMSEL_R = 0x00;
	GPIO_PORTM_AMSEL_R = 0x00;
	GPIO_PORTN_AMSEL_R = 0x00;
	GPIO_PORTF_AHB_AMSEL_R = 0x00;
	GPIO_PORTP_AMSEL_R = 0x00;
	GPIO_PORTQ_AMSEL_R = 0x00;
	
		
	// 3. Limpar PCTL para selecionar o GPIO
	GPIO_PORTA_AHB_PCTL_R = 0x11;
	GPIO_PORTE_AHB_PCTL_R = 0x00;
	GPIO_PORTH_AHB_PCTL_R = 0x00;
	GPIO_PORTJ_AHB_PCTL_R = 0x00;
	GPIO_PORTK_PCTL_R = 0x00;
	GPIO_PORTL_PCTL_R = 0x00;
	GPIO_PORTM_PCTL_R = 0x00;
	GPIO_PORTN_PCTL_R = 0x00;
	GPIO_PORTF_AHB_PCTL_R = 0x00;
	GPIO_PORTP_PCTL_R = 0x00;
	GPIO_PORTQ_PCTL_R = 0x00;

	// 4. DIR para 0 se for entrada, 1 se for saída 
	//GPIO_PORTA_AHB_DIR_R = 0xF0; // 2_1111 0011
	GPIO_PORTE_AHB_DIR_R = ~0x10u; // 2_1110 1111 
	GPIO_PORTH_AHB_DIR_R = 0x0F; // 2_0000 1111
	GPIO_PORTJ_AHB_DIR_R = 0x00; // 2_0000 0000 
	GPIO_PORTF_AHB_DIR_R = 0x13; // 2_0001 0011
	GPIO_PORTK_DIR_R = 0xFF; // 2_1111 1111
	GPIO_PORTL_DIR_R = 0xF0; // 2_1111 0000
	GPIO_PORTM_DIR_R = 0xFF; // 2_1111 1111
	GPIO_PORTN_DIR_R = 0x03; //BIT0 | BIT1
	GPIO_PORTP_DIR_R = 0x20; // 2_0010 0000
	GPIO_PORTQ_DIR_R = 0x0F; // 2_0000 1111
	
		
	// 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem função alternativa	
	GPIO_PORTA_AHB_AFSEL_R = 0x03;
	GPIO_PORTE_AHB_AFSEL_R = 0x10;
	GPIO_PORTH_AHB_AFSEL_R = 0x00;
	GPIO_PORTJ_AHB_AFSEL_R = 0x00;
	GPIO_PORTK_AFSEL_R = 0x00;
	GPIO_PORTL_AFSEL_R = 0x00;
	GPIO_PORTM_AFSEL_R = 0x00;
	GPIO_PORTN_AFSEL_R = 0x00;
	GPIO_PORTF_AHB_AFSEL_R = 0x00;
	GPIO_PORTP_AFSEL_R = 0x00;
	GPIO_PORTQ_AFSEL_R = 0x00;
		
	// 6. Setar os bits de DEN para habilitar I/O digital
	GPIO_PORTA_AHB_DEN_R = 0xF3; // 2_1111 0011
	GPIO_PORTE_AHB_DEN_R = ~0x10u; // 2_1110 1111 
	GPIO_PORTH_AHB_DEN_R = 0xF; // 2_0000 1111
	GPIO_PORTJ_AHB_DEN_R = 0x01; // 2_0000 0001 
	GPIO_PORTF_AHB_DEN_R = 0x11; // BIT4 | BIT 0
	GPIO_PORTK_DEN_R = 0xFF; // 2_1111 1111
	GPIO_PORTL_DEN_R = 0xF; // 2_0000 1111
	GPIO_PORTM_DEN_R = 0xFF; // 2_1111 1111
	GPIO_PORTN_DEN_R = 0x03; //BIT0 | BIT1
	GPIO_PORTP_DEN_R = 0x20; // 2_0010 0000
	GPIO_PORTQ_DEN_R = 0x0F; // 2_0000 1111
	
	// 7. Habilitar resistor de pull-up interno, setar PUR para 1
	GPIO_PORTJ_AHB_PUR_R = 0x01;	
	GPIO_PORTL_PUR_R = 0xF;
	
	// 7.5. ADC
	SYSCTL_RCGCADC_R = SYSCTL_RCGCADC_R | 0x1;
	while ((SYSCTL_PRADC_R & 0x1) == 0) {}
		
	ADC0_SSPRI_R = (ADC0_SSPRI_R & ~(0x3000u)) | 0x300 | 0x10 | 0x2;
	ADC0_ACTSS_R = ADC0_ACTSS_R & ~(0x8u);
	ADC0_EMUX_R = ADC0_EMUX_R & ~(0xF000u);
	ADC0_SSMUX3_R = ADC0_SSMUX3_R | 0x9;
	ADC0_SSCTL3_R = ADC0_SSCTL3_R | 0x6;
	ADC0_ACTSS_R = ADC0_ACTSS_R | 0x8;
	//ADC0_EMUX_R = ADC0_EMUX_R & ~0xF;
	
	// 8. Timer
	SYSCTL_RCGCTIMER_R = SYSCTL_RCGCTIMER_R | 0x6;
	while ((SYSCTL_PRTIMER_R & 0x6) == 0){};
	
	TIMER1_CTL_R = TIMER1_CTL_R & (~0x1u);
	TIMER2_CTL_R = TIMER2_CTL_R & (~0x1u);
	
	TIMER1_CFG_R = TIMER1_CFG_R & ~(0x7u);
	TIMER2_CFG_R = TIMER2_CFG_R & ~(0x7u);
		
	TIMER1_TAMR_R = (TIMER1_TAMR_R & ~(0x3u)) | 0x2;
	TIMER2_TAMR_R = (TIMER2_TAMR_R & ~(0x3u)) | 0x2;//0x2;
		
	TIMER1_TAILR_R = 0x13880;
	//TIMER1_TAILR_R = 0x13880111;
	TIMER2_TAILR_R = 0x3D08FF;
		
	TIMER1_TAPR_R = 0x0;
	TIMER2_TAPR_R = 0x0;
	
	TIMER1_ICR_R = TIMER1_ICR_R | 0x1;
	TIMER2_ICR_R = TIMER2_ICR_R | 0x1;
		
	TIMER1_IMR_R = TIMER1_IMR_R | 0x1;
	TIMER2_IMR_R = TIMER2_IMR_R | 0x1;
		
	NVIC_EN0_R = NVIC_EN0_R | (0x1 << 21) | (0x1 << 23);
	NVIC_PRI5_R = NVIC_PRI5_R | ((uint32_t)4u << 13) | ((uint32_t)4u << 29);
	
	
	// 9.Interrupt
	GPIO_PORTJ_AHB_IM_R = (GPIO_PORTJ_AHB_IM_R & ~0x1);
	GPIO_PORTJ_AHB_IS_R = (GPIO_PORTJ_AHB_IS_R & ~0x1);
	GPIO_PORTJ_AHB_IBE_R = (GPIO_PORTJ_AHB_IBE_R & ~0x1);
	GPIO_PORTJ_AHB_IEV_R = (GPIO_PORTJ_AHB_IEV_R & ~0x1);
	GPIO_PORTJ_AHB_ICR_R = (GPIO_PORTJ_AHB_ICR_R & ~0x1) | 0x1;
	GPIO_PORTJ_AHB_IM_R = (GPIO_PORTJ_AHB_IM_R & ~0x1) | 0x1;
	
	NVIC_EN1_R = NVIC_EN1_R | 0x80000;
	NVIC_PRI12_R = NVIC_PRI12_R | ((uint32_t)5 << 29);
}	


void PortA_Output(uint32_t valor){
	  uint32_t temp;
    //vamos zerar somente os bits menos significativos
    //para uma escrita amigável nos bits 0 e 1
    temp = GPIO_PORTA_AHB_DATA_R & (~0xF0);
    //agora vamos fazer o OR com o valor recebido na função
    temp = temp | valor;
    GPIO_PORTA_AHB_DATA_R = temp; 
}

// -------------------------------------------------------------------------------
// Função PortJ_Input
// Lê os valores de entrada do port J
// Parâmetro de entrada: Não tem
// Parâmetro de saída: o valor da leitura do port
uint32_t PortJ_Input(void)
{
	return GPIO_PORTJ_AHB_DATA_R;
}





// -------------------------------------------------------------------------------
// Função PortN_Output
// Escreve os valores no port N
// Parâmetro de entrada: Valor a ser escrito
// Parâmetro de saída: não tem
void PortN_Output(uint32_t valor)
{
    uint32_t temp;
    //vamos zerar somente os bits menos significativos
    //para uma escrita amigável nos bits 0 e 1
    temp = GPIO_PORTN_DATA_R & 0xFC;
    //agora vamos fazer o OR com o valor recebido na função
    temp = temp | valor;
    GPIO_PORTN_DATA_R = temp; 
}

// -------------------------------------------------------------------------------
// Função PortN_Output
// Escreve os valores no port N
// Parâmetro de entrada: Valor a ser escrito
// Parâmetro de saída: não tem
void PortF_Output(uint32_t valor)
{
    uint32_t temp;
    //vamos zerar somente os bits menos significativos
    //para uma escrita amigável nos bits 0 e 1
    temp = GPIO_PORTF_AHB_DATA_R & (~0x11);
    //agora vamos fazer o OR com o valor recebido na função
    temp = temp | valor;
    GPIO_PORTF_AHB_DATA_R = temp; 
}




; -------------------------------------------------------------------------------
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------
		

; -------------------------------------------------------------------------------
; Área de Dados - Declarações de variáveis
		AREA  DATA, ALIGN=2
		; Se alguma variável for chamada em outro arquivo
		;EXPORT  <var> [DATA,SIZE=<tam>]   ; Permite chamar a variável <var> a 
		                                   ; partir de outro arquivo
;<var>	SPACE <tam>                        ; Declara uma variável de nome <var>
                                           ; de <tam> bytes a partir da primeira 
                                           ; posição da RAM		
; -------------------------------------------------------------------------------
; Área de Código - Tudo abaixo da diretiva a seguir será armazenado na memória de 
;                  código
        AREA    |.text|, CODE, READONLY, ALIGN=2

		EXPORT MainLoop
		
        EXPORT Start                ; Permite chamar a função Start a partir de 
			                        ; outro arquivo. No caso startup.s
									
		; Se chamar alguma função externa	
        ;IMPORT <func>              ; Permite chamar dentro deste arquivo uma 
									; função <func>
								
		EXPORT zerar_memoria
		IMPORT PLL_Init
		IMPORT SysTick_Init
		IMPORT SysTick_Wait1ms			
		
		IMPORT GPIO_Init
		IMPORT PortA_Output
		IMPORT PortP_Output
		IMPORT PortQ_Output
			
		IMPORT LCD_Init
		IMPORT LCD_Line2
		IMPORT LCD_Reset
		IMPORT LCD_PrintString
		
		IMPORT MapMatrixKeyboard
		IMPORT print_tabuada
		IMPORT update_tabuada
		EXPORT BlinkLED
						
; -------------------------------------------------------------------------------
Start  		
	BL PLL_Init				; Chama a subrotina para alterar o clock do microcontrolador para 80MHz
	BL SysTick_Init			; Chama a subrotina para inicializar o SysTick
	BL GPIO_Init			; Chama a subrotina que inicializa os GPIO
	BL LCD_Init				; Chama a subrotina que inicializa o LCD
	
	BL zerar_memoria
	
	BL LCD_Reset				; Limpa o display e coloca o cursor em home	
	LDR R4, =TABUADA_INIT1
	BL LCD_PrintString
	BL LCD_Line2
	LDR R4, =TABUADA_INIT2
	BL LCD_PrintString
	
	MOV R12, #0
	MOV R11, #0
	MOV R10, #0
	MOV R10, #0
	
	B MainLoop

; Função MainLoop
; Loop principal do programa
; Parâmetro de entrada: R5 -> Estado atual do cofre
; Parâmetro de saída: Não tem
MainLoop
	BL MapMatrixKeyboard
	

	B MainLoop


BlinkLED
	PUSH {LR}
	
	MOV R0, #2_100000			; Ativa o transistor dos LEDs (PP5)
	BL PortP_Output
	
	ADD R10, #1
	
	CMP R10, R12
	ITT GT
		MOVGT R10, #1
		EORGT R9, #2_11111111 
	
	MOV R0, #1					; Atrasa 1ms
	BL SysTick_Wait1ms
	
	MOV R8, R9
	CMP R11, #9
	ITTE NE
		MOVNE R2, #8
		SUBNE R2, R11
		MOVEQ R2, #0
	
	LSR R1, R9, R2
	
	MOV R0, R1			; Ativa os LEDs PA7:PA4
	BL PortA_Output
	MOV R0, R1			; Ativa os LEDs PQ3:PQ0
	BL PortQ_Output
	
	POP {LR}
	BX LR

zerar_memoria
    LDR R0, =0x20000A00      ; Endereço inicial
    LDR R1, =0x20000B00      ; Endereço final
    MOV R2, #9               ; Valor a ser escrito (zero)

zerar_loop
    CMP R0, R1                ; Verifica se R0 atingiu o endereço final
    BGE zerar_fim             ; Se R0 >= R1, sai do loop
    STR R2, [R0], #4          ; Armazena zero no endereço atual e incrementa R0 em 4 bytes
    B zerar_loop              ; Repete o loop

zerar_fim
    BX LR                     ; Retorna da função
	

; Definição dos textos do LCD com 16 caracteres cada
TABUADA_INIT1 DCB "Tabuada do nxm", 0
TABUADA_INIT2 DCB "nxm=resultado", 0

EMPTY_STR	DCB "                ", 0

; -------------------------------------------------------------------------------------------------------------------------
; Fim do Arquivo
; -------------------------------------------------------------------------------------------------------------------------	
    ALIGN					; Garante que o fim da seção está alinhada 
    END						; Fim do arquivo
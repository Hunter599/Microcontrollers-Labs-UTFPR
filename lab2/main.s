
; -------------------------------------------------------------------------------
        THUMB                        ; Instru��es do tipo Thumb-2
; -------------------------------------------------------------------------------
		

; -------------------------------------------------------------------------------
; �rea de Dados - Declara��es de vari�veis
		AREA  DATA, ALIGN=2
		; Se alguma vari�vel for chamada em outro arquivo
		;EXPORT  <var> [DATA,SIZE=<tam>]   ; Permite chamar a vari�vel <var> a 
		                                   ; partir de outro arquivo
;<var>	SPACE <tam>                        ; Declara uma vari�vel de nome <var>
                                           ; de <tam> bytes a partir da primeira 
                                           ; posi��o da RAM		
; -------------------------------------------------------------------------------
; �rea de C�digo - Tudo abaixo da diretiva a seguir ser� armazenado na mem�ria de 
;                  c�digo
        AREA    |.text|, CODE, READONLY, ALIGN=2

		EXPORT MainLoop
		
        EXPORT Start                ; Permite chamar a fun��o Start a partir de 
			                        ; outro arquivo. No caso startup.s
									
		; Se chamar alguma fun��o externa	
        ;IMPORT <func>              ; Permite chamar dentro deste arquivo uma 
									; fun��o <func>
								
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

; Fun��o MainLoop
; Loop principal do programa
; Par�metro de entrada: R5 -> Estado atual do cofre
; Par�metro de sa�da: N�o tem
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
    LDR R0, =0x20000A00      ; Endere�o inicial
    LDR R1, =0x20000B00      ; Endere�o final
    MOV R2, #9               ; Valor a ser escrito (zero)

zerar_loop
    CMP R0, R1                ; Verifica se R0 atingiu o endere�o final
    BGE zerar_fim             ; Se R0 >= R1, sai do loop
    STR R2, [R0], #4          ; Armazena zero no endere�o atual e incrementa R0 em 4 bytes
    B zerar_loop              ; Repete o loop

zerar_fim
    BX LR                     ; Retorna da fun��o
	

; Defini��o dos textos do LCD com 16 caracteres cada
TABUADA_INIT1 DCB "Tabuada do nxm", 0
TABUADA_INIT2 DCB "nxm=resultado", 0

EMPTY_STR	DCB "                ", 0

; -------------------------------------------------------------------------------------------------------------------------
; Fim do Arquivo
; -------------------------------------------------------------------------------------------------------------------------	
    ALIGN					; Garante que o fim da se��o est� alinhada 
    END						; Fim do arquivo
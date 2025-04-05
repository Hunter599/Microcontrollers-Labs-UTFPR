; main.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron
; 24/08/2020
; Este programa espera o usuário apertar a chave USR_SW1.
; Caso o usuário pressione a chave, o LED1 piscará a cada 0,5 segundo.

; -------------------------------------------------------------------------------
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------
		
; Declarações EQU - Defines
;<NOME>         EQU <VALOR>
; ========================

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

		; Se alguma função do arquivo for chamada em outro arquivo	
        EXPORT Start                ; Permite chamar a função Start a partir de 
			                        ; outro arquivo. No caso startup.s
									
		; Se chamar alguma função externa	
        ;IMPORT <func>              ; Permite chamar dentro deste arquivo uma 
									; função <func>
		IMPORT  PLL_Init
		IMPORT  SysTick_Init
		IMPORT  SysTick_Wait1ms			
		IMPORT  GPIO_Init
		IMPORT  PortP_Output
		IMPORT  PortQ_Output
		IMPORT  PortA_Output
		IMPORT  PortB_Output
		IMPORT  PortJ_Input
        
		
start_7seg_mem EQU 0x20000A00 
end_7seg_mem EQU 0x20000A24   
	
start_led_mem EQU 0x20000C00   
end_led_mem EQU 0x20000C24     

tranP EQU 2_00100000
tranB4 EQU 2_00010000
tranB5 EQU 2_00100000			

waitTime EQU 5
delayTime EQU 5


mem7seg_init 
	LDR R12, =start_7seg_mem
	
	;7seg1 (second 7 segment display)
	
	MOV R0, #2_00111111 ;0
	STR R0, [R12]
	
	MOV R0, #2_00000110 ;1
	STR R0, [R12, #4]!
	
	MOV R0, #2_01011011 ;2
	STR R0, [R12, #4]!
	
	MOV R0, #2_01001111 ;3
	STR R0, [R12, #4]!
	
	MOV R0, #2_01100110 ;4
	STR R0, [R12, #4]!
	
	MOV R0, #2_01101101 ;5
	STR R0, [R12, #4]!
	
	MOV R0, #2_01111101 ;6
	STR R0, [R12, #4]!
	
	MOV R0, #2_00000111 ;7
	STR R0, [R12, #4]!
	
	MOV R0, #2_01111111 ;8
	STR R0, [R12, #4]!
	
	MOV R0, #2_01101111 ;9
	STR R0, [R12, #4]!
	
	
	
	BX LR
; -------------------------------------------------------------------------------
; Função main()
Start  		
	BL PLL_Init                  ;Chama a subrotina para alterar o clock do microcontrolador para 80MHz
	BL SysTick_Init
	BL GPIO_Init  	;Chama a subrotina que inicializa os GPIO
	
	BL mem7seg_init
	
	MOV R10, #0 ;Counter
	MOV R11, #1  ;Step
	MOV R8, #0 ;Clock
	
MainLoop
	
	
	CMP R8, #100
	ADD R8, #1
	ITT EQ
		MOVEQ R8, #0
		BLEQ updateNumber
	BL seg7_show
	BL showLeds
	
	
	
	B MainLoop                   ;Volta para o laço principal


; -------------------------------------------------------------------------------
; Updates the count for 7 segment led display consistency
; Parâmetro de entrada: -
; Parâmetro de saída: -
updateNumber
	PUSH {LR}
	
	BL count
	BL checkCounter
	BL checkButtons
	BL checkStepper
	
	POP{LR}
	BX LR
	
	
	
; -------------------------------------------------------------------------------
; Checks for overflown and underflow for the stepper
; Parâmetro de entrada: -
; Parâmetro de saída: -
checkStepper
	PUSH {LR}
	
	CMP R11, #0
	BGT overStep
	
	;underStep
	CMP R11, #-9
	BLT under_reset
	
	POP {LR}
	BX LR
	
under_reset
	ADD R11, #9
	
	POP {LR}
	BX LR

overStep
	CMP R11, #9
	BGT over_reset
	
	POP {LR}
	BX LR

over_reset
	SUB R11, #9
	
	POP {LR}
	BX LR


; -------------------------------------------------------------------------------
; Checks if buttons status changed
; Parâmetro de entrada: -
; Parâmetro de saída: -
checkButtons
	PUSH {LR}
	
	
	BL PortJ_Input				 ;Chama a subrotina que lê o estado das chaves e coloca o resultado em R0
	;LSR R5, R0, #0
	MOV R5, R0
	LSL R5, #31
	
	CMP R5, R6
	BLGT stepper
	MOV R6, R5	
	
	LSR R5, R0, #1
	LSL R5, #31
	
	CMP R5, R7
	BLGT switchStep
	MOV R7, R5
	
	
	POP {LR}
	BX LR
	
; -------------------------------------------------------------------------------
; Steps up or down the step
; Parâmetro de entrada: R11
; Parâmetro de saída: -

stepper
	PUSH {LR}
	
	CMP R11, #0
	BGT stepUp
	
;stepDown
	ADD R11, #-1
	
	POP {LR}
	BX LR

stepUp
	ADD R11, #1
	
	POP {LR}
	BX LR
	

; -------------------------------------------------------------------------------
; Checks for underflow and overflows
; Parâmetro de entrada: R10
; Parâmetro de saída: -

checkCounter
	PUSH {LR}
	
	CMP R11, #0
	BGT overflow
	
;Underflow
	CMP R10, #0 
	BLE desc_reset
	
	POP {LR}
	BX LR ;out
	
desc_reset
	ADD R10, #100
	
	POP {LR}
	BX LR ;out
	
overflow
	CMP R10, #99 
	BGE asc_reset
	
	POP {LR}
	BX LR ;out

asc_reset
	SUB R10, #100
	POP {LR}
	BX LR

; -------------------------------------------------------------------------------
; Lits up Leds
; Parâmetro de entrada: R10
; Parâmetro de saída: 
showLeds
	PUSH {LR}
	
	MOV R0, R10
	PUSH {R0}
	
	LDR R0, =tranP
	BL PortP_Output
	
	POP{R0}
	BL PortQ_Output
	BL PortA_Output
	MOV R0, #1                ;Chamar a rotina para esperar 0,01s
	BL SysTick_Wait1ms
	
	MOV R0, #0
	BL PortP_Output
	
	
	
	POP {LR}
	BX LR

; -------------------------------------------------------------------------------
; Counts up or down
; Parâmetro de entrada: R10
; Parâmetro de saída: 
count
	PUSH {LR}
	CMP R11, #0
	BGT asc
	ADD R10, R11
	POP {LR}
	BX LR
asc
	ADD R10, R11
	POP {LR}
	BX LR

; -------------------------------------------------------------------------------
; Switches the step "direction"
; Parâmetro de entrada: R11
; Parâmetro de saída: -
switchStep
	PUSH {LR}
	RSB R11, R11, #0	
	POP{LR}
	BX LR

; -------------------------------------------------------------------------------
; Show number in 7 segment display
; Parâmetro de entrada: R10
; Parâmetro de saída: R1 --> binario em formato SSD
seg7_show
	
	PUSH {LR}
	
	MOV R0, R10
	BL bin2ssd
	MOV R0, R3
	
	PUSH {R0}
	;Activates part A of the 7 segment
	LDR R0, =tranB5			;Activates a single transistor
	BL PortB_Output	;Chamar a função para acender o transistor de Leds
	POP{R0}
	BL PortQ_Output
	BL PortA_Output
	MOV R0, #waitTime                ;Chamar a rotina para esperar 0,01s
	BL SysTick_Wait1ms
	
	
	
	
	MOV R0, #0			;Activates a single transistor
	BL PortB_Output				;Chamar a função para acender o transistor de Leds
	
	MOV R0, R10
	BL bin2ssd
	MOV R0, R2
	
	PUSH {R0}
	
	;Activates part B of the 7 segment
	LDR R0, =tranB4			;Activates a single transistor
	BL PortB_Output			;Chamar a função para acender o transistor de Leds
	POP{R0}
	BL PortQ_Output
	BL PortA_Output
	MOV R0, #1                ;Chamar a rotina para esperar 0,01s
	BL SysTick_Wait1ms
	
	;Deactivates part B of the 7 segment
	MOV R0, #0			;Activates a single transistor
	BL PortB_Output			;Chamar a função para acender o transistor de Leds
	
	
	POP {LR}
	BX LR
	
; -------------------------------------------------------------------------------
; Bin to SSD
; Parâmetro de entrada: R0
; Parâmetro de saída: R1 --> binario em formato SSD
bin2ssd
	MOV 	R1, #100		; 
	UDIV	R3, R0, R1  
	MLS		R0, R3, R1, R0 
	MOV 	R1, #10
	UDIV	R2, R0, R1  ; MSB
	MLS		R3, R2, R1, R0 ; LSB
	LDR		R12, =start_7seg_mem ;start of 7 segment display memory
	MOV 	R4, #4 				 ;memory constant for each adress	
	MLA		R12, R4, R2, R12
	LDR		R2, [R12]				;SSD for MSB
	
	LDR		R12, =start_7seg_mem ;start of 7 segment display memory
	MLA		R12, R4, R3, R12
	LDR		R3, [R12]				;SSD for LSB
	
	
	BX LR
	
	

; -------------------------------------------------------------------------------------------------------------------------
; Fim do Arquivo
; -------------------------------------------------------------------------------------------------------------------------	
    ALIGN                        ;Garante que o fim da seção está alinhada 
    END                          ;Fim do arquivo

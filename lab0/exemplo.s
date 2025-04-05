; Exemplo.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron
; 12/03/2018

; -------------------------------------------------------------------------------
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------
; Declarações EQU - Defines
;<NOME>         EQU <VALOR>
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

; -------------------------------------------------------------------------------
; Função main()
;LAB 0 Bruno Ledesma, Felipe Eufranio, Victor Damasceno 22/10
desord EQU 0x20000A00
ord EQU 0x20000B00	
Start
	LDR R0, =desord
	MOV R1, #193  ; START LOADING NUMBERS
	STRB R1, [R0], #1
	MOV R1, #63 
	STRB R1, [R0], #1
	MOV R1, #176 
	STRB R1, [R0], #1
	MOV R1, #127 
	STRB R1, [R0], #1
	MOV R1, #43 
	STRB R1, [R0], #1
	MOV R1, #13 
	STRB R1, [R0], #1
	MOV R1, #211 
	STRB R1, [R0], #1
	MOV R1, #3 
	STRB R1, [R0], #1
	MOV R1, #203 
	STRB R1, [R0], #1
	MOV R1, #5 
	STRB R1, [R0], #1
	MOV R1, #21
	STRB R1, [R0], #1
	MOV R1, #7 
	STRB R1, [R0], #1
	MOV R1, #206 
	STRB R1, [R0], #1
	MOV R1, #245 
	STRB R1, [R0], #1
	MOV R1, #157 
	STRB R1, [R0], #1
	MOV R1, #237 
	STRB R1, [R0], #1
	MOV R1, #241 
	STRB R1, [R0], #1
	MOV R1, #105 
	STRB R1, [R0], #1
	MOV R1, #252 
	STRB R1, [R0], #1
	MOV R1, #19 
	STRB R1, [R0], #1 ;FINISH LOADING NUMBERS
	LDR R0, =desord
	LDR R1, =ord
	MOV R2, #0 ; INIT LIST SIZE
	LDRB R5, [R0], #1
is_prime; verify if the value in R5 is prime and store result in R6
	MOV R6, #1
	CMP R5, #1
	MOVEQ R6, #0
	MOV R3, #2
loop_ints
	UDIV R4, R5, R3
	MLS R4, R3, R4, R5
	CMP R4, #0
	MOVEQ R6, #0
	ADD R3, #1
	CMP R3, R5
	BLT loop_ints
	CMP R5, #2
	MOVEQ R6, #1
	CMP R6, #1
	ITT EQ ;IF PRIME STORE IN RAM
	STRBEQ R5, [R1], #1  
	ADDEQ R2, #1 ; INCREMENT LIST SIZE
	LDRB R5, [R0], #1 ;LOAD NEXT NUMBER
	CMP R5, #0 ;IF NEXT NUMBER = 0, END LOOP
	BNE is_prime 
	
bubbleout
	LDR R0, =ord
	MOV R10, #0 ;R10: flag isOrdered
	MOV R11, #1 ;R11: i
bubblein
	ADD R11, R11, #1 ; i++
	LDRB R8, [R0], #1 ;R8 e R9: swap
	LDRB R9, [R0]
	CMP R8, R9
	ITTT GE
		STRBGE R8, [R0]
		STRBGE R9, [R0, #-1] 
		MOVGE R10, #1
	CMP R11, R2 ; (i==listSize)?
	BNE bubblein
	CMP R10, #1 
	BEQ bubbleout
	NOP	
	ALIGN                           ; garante que o fim da seção está alinhada 
    END                             ; fim do arquivo

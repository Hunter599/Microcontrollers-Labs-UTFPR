#include <stdint.h>
#include <stdbool.h>
#include "matriz_keyboard.h"
#include "gpio.h"
#include "lcd.h"


// Constants
#define start_mem 0x20000A00
#define FULLSTEP 0
#define HALFSTEP 1

// Global variable(s)
int32_t degrees = 0;
uint32_t laps = 0;
uint32_t motor_State =0; // 0 Full, 1 Half
uint32_t dir =0; // 0 Clockwise, 1 Counter Clockwise

void SysTick_Wait1ms(uint32_t delay);
void calculate_steps(uint32_t input_degrees);
void Control_Stepper_Motor(uint32_t direction, uint32_t stepMode);


void DIGIT_0() {
    Debouncing();
    process_digit(0);
}

void DIGIT_1() {
    Debouncing();
    process_digit(15);
		calculate_steps(15);
		dir =0;
}

void DIGIT_2() {
    Debouncing();
    process_digit(30);
		calculate_steps(30);
		dir =0;
}

void DIGIT_3() {
    Debouncing();
    process_digit(45);
		calculate_steps(45);
		dir =0;
}

void DIGIT_4() {
    Debouncing();
    process_digit(60);
		calculate_steps(60);
		dir =0;
}

void DIGIT_5() {
    Debouncing();
    process_digit(90);
		calculate_steps(90);
		dir =0;
}

void DIGIT_6() {
    Debouncing();
    process_digit(180);
		calculate_steps(180);
		dir =0;
}

void DIGIT_7() {
    Debouncing();
    process_digit(-15);
		calculate_steps(15);
		dir =1;
}

void DIGIT_8() {
    Debouncing();
    process_digit(-30);
		calculate_steps(30);
		dir =1;
}

void DIGIT_9() {
    Debouncing();
    process_digit(-45);
		calculate_steps(45);
		dir =1;
}

void DIGIT_A() {
    Debouncing();
    process_digit(-60);
		calculate_steps(60);
		dir =1;
}

void DIGIT_B() {
    Debouncing();
    process_digit(-90);
		calculate_steps(90);
		dir =1;
}

void DIGIT_C() {
    Debouncing();
    process_digit(-180);
		calculate_steps(180);
		dir =1;
}


void DIGIT_AST() {
    // Additional logic for DIGIT_AST
    Debouncing();
}

// Function to handle debouncing
void Debouncing(void) {
    SysTick_Wait1ms(500); // Wait for 500 ms
}

// Function to process a digit
void process_digit(int32_t digit) {
    update_degrees(digit);         // Update the tabuada
    print_status(digit);           // Print the tabuada
	
	
}


// Function MapMatrixKeyboard
// Maps the matrix keyboard
// Input Parameter:
// Output Parameter:
void MapMatrixKeyboard() {
    // Iterating over the first column
    PortM_Output(0xE0); // MOV R0, #2_11100000
    PortL_Input();
    
    if (PortL_Input() == 0xE) { // CMP R0, #2_1110
        DIGIT_1();
    } else if (PortL_Input() == 0xD) { // CMP R0, #2_1101
        DIGIT_4();
    } else if (PortL_Input() == 0xB) { // CMP R0, #2_1011
        DIGIT_7();
    } else if (PortL_Input() == 0x7) { // CMP R0, #2_0111
        DIGIT_AST();
    }
    
    // Iterating over the second column
    PortM_Output(0xD0); // MOV R0, #2_11010000
    PortL_Input();
    
    if (PortL_Input() == 0xE) { // CMP R0, #2_1110
        DIGIT_2();
    } else if (PortL_Input() == 0xD) { // CMP R0, #2_1101
        DIGIT_5();
    } else if (PortL_Input() == 0xB) { // CMP R0, #2_1011
        DIGIT_8();
    } else if (PortL_Input() == 0x7) { // CMP R0, #2_0111
        DIGIT_0();
    }
    
    // Iterating over the third column
    PortM_Output(0xB0); // MOV R0, #2_10110000
    PortL_Input();
    
    if (PortL_Input() == 0xE) { // CMP R0, #2_1110
        DIGIT_3();
    } else if (PortL_Input() == 0xD) { // CMP R0, #2_1101
        DIGIT_6();
    } else if (PortL_Input() == 0xB) { // CMP R0, #2_1011
        DIGIT_9();
    } else if (PortL_Input() == 0x7) { // CMP R0, #2_0111

    }
		
		// Iterating over the fourth column
    PortM_Output(0x70); // MOV R0, #2_01110000
    PortL_Input();
    
    if (PortL_Input() == 0xE) { // CMP R0, #2_1110
        DIGIT_A();
    } else if (PortL_Input() == 0xD) { // CMP R0, #2_1101
        DIGIT_B();
    } else if (PortL_Input() == 0xB) { // CMP R0, #2_1011
        DIGIT_C();
		}
}

void zerar_memoria(void)
{
	degrees =0;
	laps =0;
}

// Function to update the tabuada
void update_degrees(int32_t input_Degrees) 
	{
	
	degrees += input_Degrees;
	
	if (degrees >= 360)
		{
			degrees -= 360;
			laps +=1;
		}
	else if (degrees <= -360)
		{
			degrees += 360;
			laps +=1;
		}
		
    
}

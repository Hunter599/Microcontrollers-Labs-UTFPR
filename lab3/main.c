#include <stdint.h>
#include <stdio.h>

#include "gpio.h"
#include "matriz_keyboard.h"
#include "lcd.h"
#include "stepper_motor.h"




// Function prototypes for external functions
void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void Debouncing(void);


// Constants for LCD strings
const char TABUADA_INIT1[] = "Degrees   Laps   ";
const char TABUADA_INIT2[] = "Whole-Half step";
const char EMPTY_STR[] = "                ";

// Global variables (simulating registers for BlinkLED)
uint32_t R10 = 0, R11 = 0, R12 = 0, R9 = 0, R8 = 0;

//External variables por step-motor control
extern uint32_t motor_State; // 0 Full, 1 Half
extern uint32_t dir; // 0 Clockwise, 1 Counter Clockwise
extern int stepperMotorActive;

// Blink LED function
void BlinkLED(void) {
		
    PortP_Output(0x20); // Activates transistor of LEDs (PP5)
    
		uint32_t output = 0x01;
		
		PortA_Output(output); // Activates LEDs PA7:PA4
		PortQ_Output(output); // Activates LEDs PQ3:PQ0
    
		
}

#include "lcd.h"
// Main loop
int main(void) {
		
		//Start
		PLL_Init();       // Set microcontroller clock to 80MHz
    SysTick_Init();   // Initialize SysTick
    GPIO_Init();      // Initialize GPIO
    LCD_Init();       // Initialize LCD
    zerar_memoria();  // Clear memory

    LCD_Reset();      // Clear LCD and set cursor to home
    LCD_PrintString(TABUADA_INIT1); 
    LCD_Line2();
    LCD_PrintString(TABUADA_INIT2);

    R12 = R11 = R10 = 0;
		
		
    while (1) 
			{
				Stepper_Motor_Init();
        MapMatrixKeyboard();
				Control_Stepper_Motor(dir, motor_State);
			  stepperMotorActive = 0;
						
			}
		return 0;
}

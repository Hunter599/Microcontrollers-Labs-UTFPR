#include <stdint.h>
#include "gpio.h"
#include "lcd.h"

void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);

#define DIGIT_STRINGS {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"}
#define DEGREES "Deg "
#define LAPS " Laps "
#define MOTOR_HALF " Meio-Passo"
#define MOTOR_FULL " Passo-Completo"
#define EQUAL "="
#define MINUS "-"

// External variables
extern int32_t degrees;
extern uint32_t laps;
extern uint32_t motor_State; // 0 Full, 1 Half

void LCD_Instruction(uint8_t instruction) {
    PortM_Output(0x04);  // EN=1, RW=0, RS=0 (Instruction mode)
    PortK_Output(instruction);
    SysTick_Wait1ms(10); // 10ms delay
    PortM_Output(0x00);  // EN=0, RW=0, RS=0 (Disable instruction mode)
}

void LCD_Init(void) {
    LCD_Instruction(0x38); // 2-line mode, 5x7 character matrix
    LCD_Instruction(0x06); // Cursor auto-increment
    LCD_Instruction(0x0E); // Display on, cursor on, no blink
    LCD_Instruction(0x01); // Clear display and home cursor
}



void LCD_Data(uint8_t data) {
    PortM_Output(0x05);  // EN=1, RW=0, RS=1 (Data mode)
    PortK_Output(data);
    SysTick_Wait1ms(10); // 10ms delay
    PortM_Output(0x00);  // EN=0, RW=0, RS=0 (Disable data mode)
}

void LCD_Line2(void) {
    LCD_Instruction(0xC0); // Set cursor to the second line
    SysTick_Wait1ms(10);   // 10ms delay
}

void LCD_PrintString(const char *str) {
    while (*str) {
        LCD_Data((uint8_t)*str++);
    }
}

void LCD_Reset(void) {
    LCD_Instruction(0x01); // Clear display and home cursor
    SysTick_Wait1ms(10);   // 10ms delay
}

void print_numero(int number) {
    if (number == 0) {
        LCD_PrintString("0");
        return;
    }

    char buffer[12]; // Buffer for integer to string conversion
    int i = 0;

    while (number > 0) {
        buffer[i++] = '0' + (number % 10); // Extract digit
        number /= 10;
    }

    // Reverse and print digits
    for (int j = i - 1; j >= 0; j--) {
        LCD_PrintString((char[]){buffer[j], '\0'});
    }
}

void print_status() {
    LCD_Reset();
		
		int32_t tempDeg = degrees;
	
	
    LCD_PrintString(DEGREES);
		
		if (tempDeg < 0)
			{
				tempDeg *=-1;
				LCD_PrintString(MINUS);
				print_numero(tempDeg);
			} else 
			{
				print_numero(tempDeg);
			}
		LCD_PrintString(LAPS);
		print_numero(laps);
    LCD_Line2();
		if (motor_State == 0)
			LCD_PrintString(MOTOR_FULL);
		else;
			LCD_PrintString(MOTOR_HALF);
    
    
}

#include <stdint.h>

void LCD_Instruction(uint8_t instruction);
void LCD_Init(void);
void LCD_Data(uint8_t data); 
void LCD_Line2(void); 
void LCD_PrintString(const char *str);
void LCD_Reset(void); 
void print_numero(int number);
void print_status();

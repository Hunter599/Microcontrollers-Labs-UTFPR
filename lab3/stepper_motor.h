#include <stdint.h>

void Stepper_Motor_Init(void);
void Control_Stepper_Motor(uint32_t direction, uint32_t stepMode);
void calculate_steps(uint32_t input_degrees);

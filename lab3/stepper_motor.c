#include <stdint.h>
#include <math.h>

#include "stepper_motor.h"
#include "tm4c1294ncpdt.h"

void SysTick_Wait1ms(uint32_t delay);
void PortH_Output(uint32_t degrees);

//External variables
extern int32_t degrees;
extern uint32_t laps;
extern uint32_t motor_State; // 0 Full, 1 Half
extern int stepperMotorActive;


//Global variables
uint32_t steps_full[8] = {0xC, 0x6, 0x3, 0x9};
uint32_t steps_half[8] = {0xC, 0x4, 0x6, 0x2, 0x3, 0x1, 0x9, 0x8};
uint32_t step =0;
uint32_t steps_to_do = 0;

#define DEGREE_PER_FULLSTEP 0.17578125
#define DEGREE_PER_HALFSTEP 0.087890625 

void calculate_steps(uint32_t input_degrees)
{
	
	if(motor_State) //Halfstep state = 1 else is Fullstep state
	{
		steps_to_do = ceil(input_degrees/DEGREE_PER_HALFSTEP);
	} else 
	{
		steps_to_do = ceil(input_degrees/(DEGREE_PER_FULLSTEP));
	}	
	
}

// Fun��o Stepper_Motor_Init
// Inicializa o motor zerando as fases
// Par�metro de entrada: N�o tem
// Par�metro de sa�da: N�o tem
void Stepper_Motor_Init(void)
{
	PortH_Output(0);
}

// Fun��o Control_Stepper_Motor
// Controla o motor de passo
// Par�metro de entrada: Sentido de rota��o e passo
// Par�metro de sa�da: N�o tem
void Control_Stepper_Motor(uint32_t direction, uint32_t stepMode) 
{
	
	if (steps_to_do > 0)
	{
		if (stepMode == 0)			// passo-completo
		{
			if (direction == 0)	// hor�rio
			{
				
				if (stepperMotorActive)
					{
					 
						PortH_Output(steps_full[step%4]);	
						step++;
						steps_to_do--;
						SysTick_Wait1ms(10);
					}
					
					
						

			} else if (direction == 1) // anti-hor�rio
			{
				if (stepperMotorActive)
					{
						PortH_Output(steps_full[step%4]);	
						step--;
						steps_to_do--;
						SysTick_Wait1ms(10);
					}
				
				
			}
		} else if (stepMode == 1) // meio-passo
		{
			if (direction == 0)	// hor�rio
			{
				if (stepperMotorActive)
					{
						PortH_Output(steps_half[step%8]);	
						step++;
						steps_to_do--;
						SysTick_Wait1ms(10);
					}
							
			} else if (direction == 1) // anti-hor�rio
			{
				if (stepperMotorActive)
				{
					PortH_Output(steps_half[step%8]);	
					step--;
					steps_to_do--;
					SysTick_Wait1ms(10);
				}
			}
		}
		
	}
	
}

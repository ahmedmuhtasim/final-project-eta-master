#include "buzzer.h"
#include "tm4c123gh6pm.h"

//Adapted from valvano's example in the textbook
#define duty 0x00001010
#define period 0xFFFFFFFF
void BSP_Buzzer_Init(void)
{
	SYSCTL_RCGCPWM_R |= 0x00000002; // 1) activate clock for PWM1
	while((SYSCTL_PRPWM_R&0x00000002)==0){}; // allow time to finish activating
	/*
	SYSCTL_RCGCGPIO_R |= 0x00000002; // activate clock for Port B
	while((SYSCTL_PRGPIO_R&0x00000002)==0){};
	GPIO_PORTB_AFSEL_R |= 0x40; // 2) enable alt
	GPIO_PORTB_ODR_R &= ~0x40; //disable open drain on PB6
	GPIO_PORTB_DEN_R |= 0x40;//enable digital I/O on PB6
	GPIO_PORTB_AMSEL_R &= ~0x40; //disable analog function on PB6
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xF0FFFFFF)+0x04000000;
	*/
	SYSCTL_RCGCGPIO_R |= 0x00000020; // activate clock for Port F
	while((SYSCTL_PRGPIO_R&0x00000020)==0){};
	GPIO_PORTF_AFSEL_R |= 0x04;// 2) enable alt
	GPIO_PORTF_ODR_R &= ~0x04;//disable open drain on PF2
	GPIO_PORTF_DEN_R |= 0x04;//enabledigital I/O on PF2
	GPIO_PORTF_AMSEL_R &= ~0x04;//disable analog function on PF6
	
	GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000500;//pwm
	SYSCTL_RCC_R = 0x00100000 | // 3) use PWM divider this part UNTOUCHED
		((SYSCTL_RCC_R & (~0x000E0000)) + //  clear PWM divider field
		0x00000000); // configure for /2 divider
	/*
	PWM0_0_CTL_R = 0;// 4) re-loading down-counting mode
	PWM0_0_GENA_R = 0x000000C8; // PB6 goes high on CMPA down
	PWM0_0_LOAD_R = period - 1; // 5) cycles needed to count down to 0
	PWM0_0_CMPA_R = duty - 1; // 6) count value when output rises
	PWM0_0_CTL_R |= 0x00000001; // 7) start PWM0 Generator 0
	PWM0_ENABLE_R |= 0x00000001; //enable PWM0 Generator 0
	*/
	PWM1_3_CTL_R = 0;// 4) re-loading down-counting mode
	PWM1_3_GENA_R = 0x000000C8; // PB6 goes high on CMPA down
	PWM1_3_LOAD_R = period - 1; // 5) cycles needed to count down to 0
	PWM1_3_CMPA_R = duty - 1; // 6) count value when output rises
	PWM1_3_CTL_R |= 0x00000001; // 7) start PWM1 Generator 3
	PWM1_ENABLE_R |= 0x00000040; //enable PWM0 output 6
}

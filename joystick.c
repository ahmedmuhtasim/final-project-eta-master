#include <stdint.h>
#include "joystick.h"
#include "tm4c123gh6pm.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

// There are six analog inputs on the Educational BoosterPack MKII:
// microphone (J1.6/PE5/AIN8)
// joystick X (J1.2/PB5/AIN11) and Y (J3.26/PD3/AIN4)
// accelerometer X (J3.23/PD0/AIN7), Y (J3.24/PD1/AIN6), and Z (J3.25/PD2/AIN5)
// All six initialization functions can use this general ADC
// initialization.  The joystick uses sample sequencer 1,
// the accelerometer sample sequencer 2, and the microphone
// uses sample sequencer 3.

void static adcinit(void){
  SYSCTL_RCGCADC_R |= 0x00000001;  // 1) activate ADC0
  while((SYSCTL_PRADC_R&0x01) == 0){};// 2) allow time for clock to stabilize
                                   // 3-7) GPIO initialization in more specific functions
  ADC0_PC_R &= ~0xF;               // 8) clear max sample rate field
  ADC0_PC_R |= 0x1;                //    configure for 125K samples/sec
  ADC0_SSPRI_R = 0x3210;           // 9) Sequencer 3 is lowest priority
                                   // 10-15) sample sequencer initialization in more specific functions
}

// ------------BSP_Joystick_Init------------
// Initialize a GPIO pin for input, which corresponds
// with BoosterPack pin J1.5 (Select button).
// Initialize two ADC pins, which correspond with
// BoosterPack pins J1.2 (X) and J3.26 (Y).
// Input: none
// Output: none
void BSP_Joystick_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x0000001A; // 1) activate clock for Ports E, D, and B
  while((SYSCTL_PRGPIO_R&0x1A) != 0x1A){};// allow time for clocks to stabilize
                                   // 2) no need to unlock PE4, PD3, or PB5
  GPIO_PORTE_AMSEL_R &= ~0x10;     // 3a) disable analog on PE4
  GPIO_PORTD_AMSEL_R |= 0x08;      // 3b) enable analog on PD3
  GPIO_PORTB_AMSEL_R |= 0x20;      // 3c) enable analog on PB5
                                   // 4) configure PE4 as GPIO
  GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFFF0FFFF)+0x00000000;
  GPIO_PORTE_DIR_R &= ~0x10;       // 5a) make PE4 input
  GPIO_PORTD_DIR_R &= ~0x08;       // 5b) make PD3 input
  GPIO_PORTB_DIR_R &= ~0x20;       // 5c) make PB5 input
  GPIO_PORTE_AFSEL_R &= ~0x10;     // 6a) disable alt funct on PE4
  GPIO_PORTD_AFSEL_R |= 0x08;      // 6b) enable alt funct on PD3
  GPIO_PORTB_AFSEL_R |= 0x20;      // 6c) enable alt funct on PB5
  GPIO_PORTE_DEN_R |= 0x10;        // 7a) enable digital I/O on PE4
  GPIO_PORTD_DEN_R &= ~0x08;       // 7b) enable analog functionality on PD3
  GPIO_PORTB_DEN_R &= ~0x20;       // 7c) enable analog functionality on PB5
  adcinit();                       // 8-9) general ADC initialization
  ADC0_ACTSS_R &= ~0x0002;         // 10) disable sample sequencer 1
  ADC0_EMUX_R &= ~0x00F0;          // 11) seq1 is software trigger
  ADC0_SSMUX1_R = 0x004B;          // 12) set channels for SS1
  ADC0_SSCTL1_R = 0x0060;          // 13) no TS0 D0 IE0 END0 TS1 D1, yes IE1 END1
  ADC0_IM_R &= ~0x0002;            // 14) disable SS1 interrupts
  ADC0_ACTSS_R |= 0x0002;          // 15) enable sample sequencer 1
}

// ------------BSP_Joystick_Input------------
// Read and return the immediate status of the
// joystick.  Button de-bouncing for the Select
// button is not considered.  The joystick X- and
// Y-positions are returned as 10-bit numbers,
// even if the ADC on the LaunchPad is more precise.
// Input: x is pointer to store X-position (0 to 4095)
//        y is pointer to store Y-position (0 to 4095)
//        select is pointer to store Select status (0 if pressed)
// Output: none
// Assumes: BSP_Joystick_Init() has been called
#define SELECT    (*((volatile uint32_t *)0x40024040))  /* PE4 */
void BSP_Joystick_Input(uint16_t *x, uint16_t *y, uint8_t *select){
  ADC0_PSSI_R = 0x0002;            // 1) initiate SS1
  while((ADC0_RIS_R&0x02)==0){};   // 2) wait for conversion done
  *x = ADC0_SSFIFO1_R;          // 3a) read first result
  *y = ADC0_SSFIFO1_R;          // 3b) read second result
  *select = SELECT;                // return 0(pressed) or 0x10(not pressed)
  ADC0_ISC_R = 0x0002;             // 4) acknowledge completion
}

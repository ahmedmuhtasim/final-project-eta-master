// Created by Mustafa Hotaki, July 2018

#include "PORTE.h"

void PortE_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x10;       // activate port E
  while((SYSCTL_RCGCGPIO_R & 0x10) == 0){} 
  GPIO_PORTE_DIR_R |= 0x0F;    // make PE3-0 output heartbeats
  GPIO_PORTE_AFSEL_R &= ~0x0F;   // disable alt funct on PE3-0
  GPIO_PORTE_DEN_R |= 0x0F;     // enable digital I/O on PE3-0
  GPIO_PORTE_PCTL_R = ~0x0000FFFF;
  GPIO_PORTE_AMSEL_R &= ~0x0F;      // disable analog functionality on PF
}

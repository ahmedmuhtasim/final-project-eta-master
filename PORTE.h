#ifndef __PORTE_H__
#define __PORTE_H__

#include <stdint.h>
#include "tm4c123gh6pm.h"

#define PE0  (*((volatile unsigned long *)0x40024004))
#define PE1  (*((volatile unsigned long *)0x40024008))
#define PE2  (*((volatile unsigned long *)0x40024010))
#define PE3  (*((volatile unsigned long *)0x40024020))

void PortE_Init(void);

#endif

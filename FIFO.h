// FIFO.h
// Runs on any LM3Sxxx
// Provide functions that initialize a FIFO, put data in, get data out,
// and return the current size.  The file includes a transmit FIFO
// using index implementation and a receive FIFO using pointer
// implementation.  Other index or pointer implementation FIFOs can be
// created using the macros supplied at the end of the file.
// Daniel Valvano
// May 2, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
      Programs 3.7, 3.8., 3.9 and 3.10 in Section 3.7

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#ifndef __FIFO_H__
#define __FIFO_H__

long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value

// Two-pointer implementation of the receive FIFO
// can hold 0 to RXFIFOSIZE-1 elements
#define JSFIFOSIZE 16 // can be any size
#define JSFIFOSUCCESS 1
#define JSFIFOFAIL    0

typedef struct {
	uint16_t x,y;
}  jsDataType;

// initialize pointer FIFO
void JsFifo_Init(void);
// add element to end of pointer FIFO
// return RXFIFOSUCCESS if successful
int JsFifo_Put(jsDataType data);
// remove element from front of pointer FIFO
// return RXFIFOSUCCESS if successful
int JsFifo_Get(jsDataType *datapt);
// number of elements in pointer FIFO
// 0 to RXFIFOSIZE-1
uint32_t JsFifo_Size(void);

#endif //  __FIFO_H__

// FIFO.c
// Runs on any Cortex microcontroller
// Provide functions that initialize a FIFO, put data in, get data out,
// and return the current size. 

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
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

#include <stdint.h>
#include "FIFO.h"
#include "os.h"

// Two-pointer implementation of the receive FIFO
// can hold 0 to RXFIFOSIZE-1 elements

jsDataType volatile *JsPutPt; // put next
jsDataType volatile *JsGetPt; // get next
jsDataType static JsFifo[JSFIFOSIZE];
Sema4Type JsFifoAvailable;

// initialize pointer FIFO
void JsFifo_Init(void){ long sr;
  sr = StartCritical();      // make atomic
	OS_InitSemaphore(&JsFifoAvailable, 0);
  JsPutPt = JsGetPt = &JsFifo[0]; // Empty
  EndCritical(sr);
}
// add element to end of pointer FIFO
// return RXFIFOSUCCESS if successful
int JsFifo_Put(jsDataType data){
  jsDataType volatile *nextPutPt;
  nextPutPt = JsPutPt+1;
  if(nextPutPt == &JsFifo[JSFIFOSIZE]){
    nextPutPt = &JsFifo[0];  // wrap
  }
  if(nextPutPt == JsGetPt){
    return(JSFIFOFAIL);      // Failed, fifo full
  }
  else{
    *(JsPutPt) = data;       // Put
    JsPutPt = nextPutPt;     // Success, update
		OS_Signal(&JsFifoAvailable);
    return(JSFIFOSUCCESS);
  }
}
// remove element from front of pointer FIFO
// return RXFIFOSUCCESS if successful
int JsFifo_Get(jsDataType *datapt){
  OS_Wait(&JsFifoAvailable);
  *datapt = *(JsGetPt++);
  if(JsGetPt == &JsFifo[JSFIFOSIZE]){
     JsGetPt = &JsFifo[0];   // wrap
  }
  return(JSFIFOSUCCESS);
}
// number of elements in pointer FIFO
// 0 to RXFIFOSIZE-1
uint32_t JsFifo_Size(void){
  if(JsPutPt < JsGetPt){
    return ((uint32_t)(JsPutPt-JsGetPt+(JSFIFOSIZE*sizeof(jsDataType)))/sizeof(jsDataType));
  }
  return ((uint32_t)(JsPutPt-JsGetPt)/sizeof(jsDataType));
}

// Main.c
// Runs on LM4F120/TM4C123
// You may use, edit, run or distribute this file 
// You are free to change the syntax/organization of this file

// Jonathan W. Valvano 2/20/17, valvano@mail.utexas.edu
// Modified by Sile Shu 10/4/17, ss5de@virginia.edu
// Modified by Mustafa Hotaki 7/29/18, mkh3cf@virginia.edu

#include <stdint.h>
#include "OS.h"
#include "tm4c123gh6pm.h"
#include "LCD.h"
#include <string.h> 
#include "UART.h"
#include "FIFO.h"
#include "joystick.h"
#include "PORTE.h"

// Constants
#define BGCOLOR     					LCD_BLACK
#define CROSSSIZE            	5
#define PERIOD               	4000000   // DAS 20Hz sampling period in system time units
#define PSEUDOPERIOD         	8000000
#define EXPIREPERIOD          80000
#define LIFETIME             	1000
#define RUNLENGTH            	600 // 30 seconds run length

#define VERTICALNUM 6
#define HORIZONTALNUM 6
#define NUMOFCUBES	5				// Max number of cubess
#define LIFEOFCUBE 10000		// presently 10 seconds for milisecond unit

uint16_t colors[11] = {LCD_WHITE, LCD_GREY, 
LCD_RED, LCD_GREEN, LCD_CYAN, LCD_ORANGE, 
LCD_YELLOW, LCD_LIGHTGREEN, LCD_DARKBLUE,
LCD_MAGENTA, LCD_ORANGE};

// Global variables for life and score
unsigned int life = 10;
unsigned int score = 0;

// Global variable for number of cubes present
unsigned int numCubes = 0;

// Global variable seed for PRNG
unsigned int Xn;

extern Sema4Type LCDFree;

// Struct for breaking screen into blocks
typedef struct {
 uint32_t position[2];
 Sema4Type BlockFree;
} block;

block BlockArray[HORIZONTALNUM][VERTICALNUM];

// Struct for representing a cube
typedef struct {
	unsigned int idle;	// 0 or 1
	unsigned int hit;	// 0 or 1
	unsigned int	expired; // Counts down by the 1ms software timer for each CubeArray member
	unsigned int x;	// current x
	unsigned int y; // current y
	unsigned int dir; 	// 0 = N, 1 = E, 2 = S, 3 = W
	uint16_t color;	 		// get colors from LCD.h, 
} cube;

cube CubeArray[NUMOFCUBES];

// Initialize Cube and Block Arrays
void initCubes(void) {
	int i, j;
	for(i = 0; i < NUMOFCUBES; i++) {
		CubeArray[i].idle = 1;
	}
	
	for(i = 0; i < HORIZONTALNUM; i++) {
		for(j = 0; j < VERTICALNUM; j++) {
			BlockArray[i][j].position[0] = i;
			BlockArray[i][j].position[1] = j;
			OS_InitSemaphore(&(BlockArray[i][j].BlockFree), 1);
		}
	}
}

// Seed PRNG
void seedRandom(unsigned int seed) {
	Xn = seed;
}

// Return random value from 0 to range-1
// using Linear Congruential Generator
unsigned int Random(unsigned short range)
{
	unsigned int m = 2147483647;
	unsigned int a = 16807;
	unsigned int c = 0;
	
	Xn = (a * Xn + c) % m;
	return Xn % range;
}

unsigned int GetCrossHairX()
{
	return 47;
}

unsigned int GetCrossHairY()
{
	return 47;
}

void CubeThread (void){
	// 1.allocate an idle cube for the object
	cube *thisCube = 0;		// Points to the cube assigned to this thread
	
	unsigned int n;
	unsigned int i, j;
	for (n = 0; n < NUMOFCUBES; n++){
		if (CubeArray[n].idle){
			CubeArray[n].idle = 0;
			thisCube = &(CubeArray[n]);
			break;
		}
	}
	
	if (&thisCube == 0)
		OS_Kill();				// This means that thisCube is null. That would indicate that all the cubes are taken and so this thread should be killed.
		
	// 2.initialize color/shape and the first direction
	// asign color, There may be multiples of same color (unlikely tho)...
	thisCube->color = colors[Random(11)];
	// assign shape
	
	// assign location
//	do{
//		i = Random(6); j = Random(6);
//	} while (BlockArray[i][j].BlockFree.Value == 0);
	
	i = Random(6); j = Random(6);
	OS_bWait(&(BlockArray[i][j].BlockFree));
	
	// Paint cube's initial position
	thisCube->x = i;
	thisCube->y = j;
	OS_bWait(&LCDFree);
	PaintCube(thisCube->x, thisCube->y, thisCube->color);
	OS_bSignal(&LCDFree);
	
	// assign direction
	thisCube->dir = Random(4);
	
	// cube is not hit yet
	thisCube->hit = 0;
	
	// assign lifeTime
	thisCube->expired = LIFEOFCUBE;
	
	// 3.move the cube while it is not hit or expired
	while(life){
			if(thisCube->hit){
				score++;
				OS_bWait(&LCDFree);
				PaintCube(thisCube->x, thisCube->y, LCD_BLACK);
				OS_bSignal(&LCDFree);
				OS_bSignal(&BlockArray[thisCube->x][thisCube->y].BlockFree);
				thisCube->idle = 1;
				break;
			}
			else if (thisCube->expired == 0){
				life--;
				OS_bWait(&LCDFree);
				PaintCube(thisCube->x, thisCube->y, LCD_BLACK);
				OS_bSignal(&LCDFree);
				OS_bSignal(&BlockArray[thisCube->x][thisCube->y].BlockFree);
				break;
			}
			else{
				// Moving North
				if(thisCube->dir == 0) {
					// If we've hit north wall, pick new direction
					if(thisCube->y == 0 
             || !(BlockArray[thisCube->x][thisCube->y-1].BlockFree.Value)) {
						do {
							thisCube->dir = Random(4);
						} while(thisCube->dir == 0);
					}
					else {
						OS_bWait(&LCDFree);
						PaintCube(thisCube->x, thisCube->y, LCD_BLACK);
						OS_bSignal(&(BlockArray[thisCube->x][thisCube->y].BlockFree));
						(thisCube->y)--;
						OS_bWait(&(BlockArray[thisCube->x][thisCube->y].BlockFree));
						PaintCube(thisCube->x, thisCube->y, thisCube->color);
						OS_bSignal(&LCDFree);
					}
				}
				
				// Moving West
				else if (thisCube->dir == 1) {
					// If we've hit west wall, pick new direction
					if(thisCube->x == 0
             || !(BlockArray[thisCube->x-1][thisCube->y].BlockFree.Value)) {
						do {
							thisCube->dir = Random(4);
						} while(thisCube->dir == 1);
					}
					else {
						OS_bWait(&LCDFree);
						PaintCube(thisCube->x, thisCube->y, LCD_BLACK);
						OS_bSignal(&(BlockArray[thisCube->x][thisCube->y].BlockFree));
						(thisCube->x)--;
						OS_bWait(&(BlockArray[thisCube->x][thisCube->y].BlockFree));
						PaintCube(thisCube->x, thisCube->y, thisCube->color);
						OS_bSignal(&LCDFree);
					}
				}
				
				// Moving East
				else if(thisCube->dir == 2) {
					// If we've hit east wall, pick new direction
					if(thisCube->x == HORIZONTALNUM - 1
             || !(BlockArray[thisCube->x+1][thisCube->y].BlockFree.Value)) {
						do {
							thisCube->dir = Random(4);
						} while(thisCube->dir == 2);
					}
					else {
						OS_bWait(&LCDFree);
						PaintCube(thisCube->x, thisCube->y, LCD_BLACK);
						OS_bSignal(&(BlockArray[thisCube->x][thisCube->y].BlockFree));
						(thisCube->x)++;
						OS_bWait(&(BlockArray[thisCube->x][thisCube->y].BlockFree));
						PaintCube(thisCube->x, thisCube->y, thisCube->color);
						OS_bSignal(&LCDFree);
					}
				}
				
				// Moving South
				else if (thisCube->dir == 3) {
					// If we've hit south wall, pick new direction
					if(thisCube->y == VERTICALNUM - 1
             || !(BlockArray[thisCube->x][thisCube->y+1].BlockFree.Value)) {
						do {
							thisCube->dir = Random(4);
						} while(thisCube->dir == 3);
					}
					else {
						OS_bWait(&LCDFree);
						PaintCube(thisCube->x, thisCube->y, LCD_BLACK);
						OS_bSignal(&(BlockArray[thisCube->x][thisCube->y].BlockFree));
						(thisCube->y)++;
						OS_bWait(&(BlockArray[thisCube->x][thisCube->y].BlockFree));
						PaintCube(thisCube->x, thisCube->y, thisCube->color);
						OS_bSignal(&LCDFree);
					}
				}
				
				// Make it so the blocks don't move around so fast the user
				// can't see or catch them
				OS_Sleep(50);
			}
	}
	thisCube->idle = 1;
	numCubes--;
	OS_Kill(); //Life = 0, game is over, kill the thread
}

void GenerateCubes(void) {
	int i, cubesToGenerate;
	while(life) {
		if(numCubes == 0) {
			cubesToGenerate = Random(5);
			
			for(i = 0; i < cubesToGenerate; i++) {
				numCubes++;
				OS_AddThread(&CubeThread, 128, 2);
			}
		}
		else
			OS_Suspend();
	}
}

uint16_t origin[2]; 	// The original ADC value of x,y if the joystick is not touched, used as reference
int16_t x = 63;  			// horizontal position of the crosshair, initially 63
int16_t y = 63;  			// vertical position of the crosshair, initially 63
int16_t prevx, prevy;	// Previous x and y values of the crosshair
uint8_t select;  			// joystick push
uint8_t area[2];
uint32_t PseudoCount;

unsigned long NumCreated;   		// Number of foreground threads created
unsigned long NumSamples;   		// Incremented every ADC sample, in Producer
unsigned long UpdateWork;   		// Incremented every update on position values
unsigned long Calculation;  		// Incremented every cube number calculation
unsigned long DisplayCount; 		// Incremented every time the Display thread prints on LCD 
unsigned long ConsumerCount;		// Incremented every time the Consumer thread prints on LCD
unsigned long Button1RespTime; 	// Latency for Task 2 = Time between button1 push and response on LCD 
unsigned long Button2RespTime; 	// Latency for Task 7 = Time between button2 push and response on LCD
unsigned long Button1PushTime; 	// Time stamp for when button 1 was pushed
unsigned long Button2PushTime; 	// Time stamp for when button 2 was pushed

//---------------------User debugging-----------------------
unsigned long DataLost;     // data sent by Producer, but not received by Consumer
long MaxJitter;             // largest time jitter between interrupts in usec
#define JITTERSIZE 64
unsigned long const JitterSize=JITTERSIZE;
unsigned long JitterHistogram[JITTERSIZE]={0,};
unsigned long TotalWithI1;
unsigned short MaxWithI1;

void Device_Init(void){
	UART_Init();
	BSP_LCD_OutputInit();
	BSP_Joystick_Init();
}
//------------------Task 1--------------------------------
// background thread executed at 20 Hz
//******** Producer *************** 
int UpdatePosition(uint16_t rawx, uint16_t rawy, jsDataType* data){
	if (rawx > origin[0]){
		x = x + ((rawx - origin[0]) >> 9);
	}
	else{
		x = x - ((origin[0] - rawx) >> 9);
	}
	if (rawy < origin[1]){
		y = y + ((origin[1] - rawy) >> 9);
	}
	else{
		y = y - ((rawy - origin[1]) >> 9);
	}
	if (x > 127){
		x = 127;}
	if (x < 0){
		x = 0;}
	if (y > 112 - CROSSSIZE){
		y = 112 - CROSSSIZE;}
	if (y < 0){
		y = 0;}
	data->x = x; data->y = y;
	return 1;
}

void Producer(void){
	uint16_t rawX,rawY; // raw adc value
	uint8_t select;
	jsDataType data;
	unsigned static long LastTime;  // time at previous ADC sample
	unsigned long thisTime;         // time at current ADC sample
	long jitter;                    // time between measured and expected, in us
	if (NumSamples < RUNLENGTH){
		BSP_Joystick_Input(&rawX,&rawY,&select);
		thisTime = OS_Time();       // current time, 12.5 ns
		UpdateWork += UpdatePosition(rawX,rawY,&data); // calculation work
		NumSamples++;               // number of samples
		if(JsFifo_Put(data) == 0){ // send to consumer
			DataLost++;
		}
	//calculate jitter
		if(UpdateWork > 1){    // ignore timing of first interrupt
			unsigned long diff = OS_TimeDifference(LastTime,thisTime);
			if(diff > PERIOD){
				jitter = (diff-PERIOD+4)/8;  // in 0.1 usec
			}
			else{
				jitter = (PERIOD-diff+4)/8;  // in 0.1 usec
			}
			if(jitter > MaxJitter){
				MaxJitter = jitter; // in usec
			}       // jitter should be 0
			if(jitter >= JitterSize){
				jitter = JITTERSIZE-1;
			}
			JitterHistogram[jitter]++; 
		}
		LastTime = thisTime;
	}
}

//--------------end of Task 1-----------------------------

//------------------Task 2--------------------------------
// background thread executes with SW1 button
// one foreground task created with button push
// foreground treads run for 2 sec and die
// ***********ButtonWork*************
void ButtonWork(void){
	uint32_t StartTime,CurrentTime,ElapsedTime;
	StartTime = OS_MsTime();
	ElapsedTime = 0;
	OS_bWait(&LCDFree);
	Button1RespTime = OS_MsTime() - Button1PushTime; // LCD Response here
	BSP_LCD_FillScreen(BGCOLOR);
	//Button1FuncTime = OS_MsTime() - Button1PushTime;
	//Button1PushTime = 0;
	while (ElapsedTime < LIFETIME){
		CurrentTime = OS_MsTime();
		ElapsedTime = CurrentTime - StartTime;
		BSP_LCD_Message(0,5,0,"Life Time:",LIFETIME);
		BSP_LCD_Message(1,0,0,"Horizontal Area:",area[0]);
		BSP_LCD_Message(1,1,0,"Vertical Area:",area[1]);
		BSP_LCD_Message(1,2,0,"Elapsed Time:",ElapsedTime);
		OS_Sleep(50);

	}
	BSP_LCD_FillScreen(BGCOLOR);
	OS_bSignal(&LCDFree);
  OS_Kill();  // done, OS does not return from a Kill
} 

//************SW1Push*************
// Called when SW1 Button pushed
// Adds another foreground task
// background threads execute once and return
void SW1Push(void){
  if(OS_MsTime() > 20 ){ // debounce
    if(OS_AddThread(&ButtonWork,128,4)){
			OS_ClearMsTime();
      NumCreated++; 
    }
    OS_ClearMsTime();  // at least 20ms between touches
		Button1PushTime = OS_MsTime(); // Time stamp

  }
}

//--------------end of Task 2-----------------------------

//------------------Task 3--------------------------------

//******** Consumer *************** 
// foreground thread, accepts data from producer
// Display crosshair and its positions
// inputs:  none
// outputs: none
void Consumer(void){
	unsigned int cube_width = MAX_WIDTH / 6;
	unsigned int cube_height = MAX_HEIGHT / 6;
	unsigned int xblock, yblock;
	
	while(life){
		jsDataType data;
		int i;
		JsFifo_Get(&data);
		OS_bWait(&LCDFree);
			
		BSP_LCD_DrawCrosshair(prevx, prevy, LCD_BLACK); // Draw a black crosshair
		BSP_LCD_DrawCrosshair(data.x, data.y, LCD_RED); // Draw a red crosshair
		
		if(data.x < cube_width)
			xblock = 0;
		else if(data.x < 2*cube_width)
			xblock = 1;
		else if(data.x < 3*cube_width)
			xblock = 2;
		else if(data.x < 4*cube_width)
			xblock = 3;
		else if(data.x < 5*cube_width)
			xblock = 4;
		else if(data.x < 6*cube_width)
			xblock = 5;
		
		if(data.y < cube_height)
			yblock = 0;
		else if(data.y < 2*cube_height)
			yblock = 1;
		else if(data.y < 3*cube_height)
			yblock = 2;
		else if(data.y < 4*cube_height)
			yblock = 3;
		else if(data.y < 5*cube_height)
			yblock = 4;
		else if(data.y < 6*cube_height)
			yblock = 5;
		
		for(i = 0; i < NUMOFCUBES; i++) {
			if(CubeArray[i].idle == 0) {
				if(CubeArray[i].x == xblock && CubeArray[i].y == yblock) {
					CubeArray[i].hit = 1;
				}
			}
		}
		
		BSP_LCD_Message(1, 5, 0, "Score:", score);
		BSP_LCD_Message(1, 5, 11, "Life:", life);
		ConsumerCount++;
		OS_bSignal(&LCDFree);
		prevx = data.x; 
		prevy = data.y;
		//OS_Suspend();
	}
  OS_Kill();  // done
}

void UpdateExpires(void) {
	int i;
	for(i = 0; i < NUMOFCUBES; i++) {
		if(CubeArray[i].idle == 0) {
			if(CubeArray[i].expired > 0) {
				(CubeArray[i].expired)--;
			}
		}
	}
}

//--------------end of Task 3-----------------------------

//------------------Task 4--------------------------------
// foreground thread that runs without waiting or sleeping
// it executes some calculation related to the position of crosshair 
//******** CubeNumCalc *************** 
// foreground thread, calculates the virtual cube number for the crosshair
// never blocks, never sleeps, never dies
// inputs:  none
// outputs: none

void CubeNumCalc(void){
	uint16_t CurrentX,CurrentY;
  while(1) {
		if(NumSamples < RUNLENGTH){
			CurrentX = x; CurrentY = y;
			area[0] = CurrentX / 22;
			area[1] = CurrentY / 20;
			Calculation++;
			//OS_Suspend();
		}
  }
}
//--------------end of Task 4-----------------------------

//------------------Task 5--------------------------------
// UART background ISR performs serial input/output
// Two software fifos are used to pass I/O data to foreground
// The interpreter runs as a foreground thread
// inputs:  none
// outputs: none

void Interpreter(void){
	char command[80];
  while(1){
    OutCRLF(); UART_OutString(">>");
		UART_InString(command,79);
		OutCRLF();
		if (!(strcmp(command,"NumSamples"))){
			UART_OutString("NumSamples: ");
			UART_OutUDec(NumSamples);
		}
		else if (!(strcmp(command,"NumCreated"))){
			UART_OutString("NumCreated: ");
			UART_OutUDec(NumCreated);
		}
		else if (!(strcmp(command,"MaxJitter"))){
			UART_OutString("MaxJitter: ");
			UART_OutUDec(MaxJitter);
		}
		else if (!(strcmp(command,"DataLost"))){
			UART_OutString("DataLost: ");
			UART_OutUDec(DataLost);
		}
		else if (!(strcmp(command,"UpdateWork"))){
			UART_OutString("UpdateWork: ");
			UART_OutUDec(UpdateWork);
		}
	  else if (!(strcmp(command,"Calculations"))){
			UART_OutString("Calculations: ");
			UART_OutUDec(Calculation);
		}
		else if (!(strcmp(command,"FifoSize"))){
			UART_OutString("JSFifoSize: ");
			UART_OutUDec(JSFIFOSIZE);
		}
	  else if (!(strcmp(command,"Display"))){
			UART_OutString("DisplayWork: ");
			UART_OutUDec(DisplayCount);
		}
		else if (!(strcmp(command,"Consumer"))){
			UART_OutString("ConsumerWork: ");
			UART_OutUDec(ConsumerCount);
		}
		else{
			UART_OutString("Command incorrect!");
		}
		//OS_Suspend();
  }
}
//--------------end of Task 5-----------------------------

//------------------Task 6--------------------------------

//************ PeriodicUpdater *************** 
// background thread, do some pseudo works to test if you can add multiple periodic threads
// inputs:  none
// outputs: none
void PeriodicUpdater(void){
	PseudoCount++;
}

//************ Display *************** 
// foreground thread, do some pseudo works to test if you can add multiple periodic threads
// inputs:  none
// outputs: none
void Display(void){
	while(NumSamples < RUNLENGTH){
		OS_bWait(&LCDFree);
		BSP_LCD_Message(1,4,0,"PseudoCount: ",PseudoCount);
		DisplayCount++;
		OS_bSignal(&LCDFree);
		//OS_Sleep(1);
		//OS_Suspend();

	}
  OS_Kill();  // done
}

//--------------end of Task 6-----------------------------

//------------------Task 7--------------------------------
// background thread executes with button2
// one foreground task created with button push
// ***********ButtonWork2*************
void Restart(void){
	uint32_t StartTime,CurrentTime,ElapsedTime;
	NumSamples = RUNLENGTH; // first kill the foreground threads
	OS_Sleep(50); // wait
	StartTime = OS_MsTime();
	ElapsedTime = 0;
	OS_bWait(&LCDFree);
	Button2RespTime = OS_MsTime() - Button2PushTime; // Response on LCD here
	BSP_LCD_FillScreen(BGCOLOR);
	while (ElapsedTime < 500){
		CurrentTime = OS_MsTime();
		ElapsedTime = CurrentTime - StartTime;
		BSP_LCD_DrawString(5,6,"Restarting",LCD_WHITE);
	}
	BSP_LCD_FillScreen(BGCOLOR);
	OS_bSignal(&LCDFree);
	// restart
	DataLost = 0;        // lost data between producer and consumer
  NumSamples = 0;
  UpdateWork = 0;
	MaxJitter = 0;       // in 1us units
	PseudoCount = 0;
	x = 63; y = 63;
	NumCreated += OS_AddThread(&Consumer,128,1); 
	NumCreated += OS_AddThread(&Display,128,3);
  OS_Kill();  // done, OS does not return from a Kill
} 

//************SW2Push*************
// Called when Button2 pushed
// Adds another foreground task
// background threads execute once and return
void SW2Push(void){
  if(OS_MsTime() > 20 ){ // debounce
    if(OS_AddThread(&Restart,128,4)){
			OS_ClearMsTime();
      NumCreated++; 
    }
    OS_ClearMsTime();  // at least 20ms between touches
		Button2PushTime = OS_MsTime(); // Time stamp
  }
}

//--------------end of Task 7-----------------------------

// Fill the screen with the background color
// Grab initial joystick position to bu used as a reference
void CrossHair_Init(void){
	BSP_LCD_FillScreen(BGCOLOR);
	BSP_Joystick_Input(&origin[0],&origin[1],&select);
}

//******************* Main Function**********

int main(void){
	OS_Init();           // initialize, disable interrupts
	Device_Init();
  CrossHair_Init();
  DataLost = 0;        // lost data between producer and consumer
  NumSamples = 0;
  MaxJitter = 0;       // in 1us units
	PseudoCount = 0;
	
	
//********initialize communication channels
  JsFifo_Init();

	initCubes();
	seedRandom(42);
	
//*******attach background tasks***********
  OS_AddSW1Task(&SW1Push, 4);
	OS_AddSW2Task(&SW2Push, 4);
  OS_AddPeriodicThread(&Producer, PERIOD, 3); // 2 kHz real time sampling of PD3
	OS_AddPeriodicThread(&UpdateExpires, EXPIREPERIOD, 3);
	
  NumCreated = 0 ;
// create initial foreground threads
  //NumCreated += OS_AddThread(&Interpreter, 128, 2); 
  NumCreated += OS_AddThread(&Consumer, 128, 1);
	NumCreated += OS_AddThread(&CubeNumCalc, 128, 3);
	//NumCreated += OS_AddThread(&CubeThread, 128, 1);
	//NumCreated += OS_AddThread(&CubeThread, 128, 1);
	NumCreated += OS_AddThread(&GenerateCubes, 128, 1);
	//NumCreated += OS_AddThread(&Display, 128, 3);
 
 
  OS_Launch(TIME_2MS); // doesn't return, interrupts enabled in here
	return 0;            // this never executes
}

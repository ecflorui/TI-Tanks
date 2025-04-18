// Dump.c
// Your solution to ECE319K Lab 3 Spring 2025
// Author: Krish Singh
// Last Modified: Your date


#include <ti/devices/msp/msp.h>
#include "../inc/Timer.h"
#define MAXBUF 50
uint32_t DataBuffer[MAXBUF]; //32-bit DATA
uint32_t TimeBuffer[MAXBUF]; //32-bit TIME
uint32_t DebugCnt; // 0 to MAXBUF (0 is empty, MAXBUF is full)

// *****Debug_Init******
// Initializes your index or pointer.
// Input: none
// Output:none
void Debug_Init(void){
// students write this for Lab 3
// This function should also initialize Timer G12, call TimerG12_Init.
TimerG12_Init(); //starts a timer that counts DOWN from 0xFFFFFFFF
DebugCnt = 0; //we initiall have NO values in the array
}

// *****Debug_Dump******
// Records one data and one time into the two arrays.
// Input: data is value to store in DataBuffer
// Output: 1 for success, 0 for failure (buffers full)
uint32_t Debug_Dump(uint32_t data){
// students write this for Lab 3
// The software simply reads TIMG12->COUNTERREGS.CTR to get the current time in bus cycles.
if (DebugCnt >= MAXBUF) { //if we have 50 things in the array already, we are done
  return 0; //buffer full, fail
}

DataBuffer[DebugCnt] = data; //put data into data array
TimeBuffer[DebugCnt] = TIMG12->COUNTERREGS.CTR; //put current time into time array, accessed via TIMG12 struct
DebugCnt++;

return 1; // success
}
// *****Debug_Dump2******
// Always record data and time on the first call to Debug_Dump2
// However, after the first call
//    Records one data and one time into the two arrays, only if the data is different from the previous call.
//    Do not record data or time if the data is the same as the data from the previous call
// Input: data is value to store in DataBuffer
// Output: 1 for success (saved or skipped), 0 for failure (buffers full)
uint32_t Debug_Dump2(uint32_t data){
// students write this for Lab 3
// The software simply reads TIMG12->COUNTERREGS.CTR to get the current time in bus cycles.
  
  return 1; // success
}

// *****Debug_Period******
// Calculate period of the recorded data using mask
// Input: mask specifies which bit(s) to observe
// Output: period in bus cycles
// Period is defined as rising edge (low to high) to the next rising edge.
// Return 0 if there is not enough collected data to calculate period .
uint32_t Debug_Period(uint32_t mask){
// students write this for Lab 3
// This function should not alter the recorded data.
// AND each recorded data with mask,
//    if nonzero the signal is considered high.
//    if zero, the signal is considered low.

uint32_t prevValue = 0; //records last ANDED value
uint32_t prevRiseTime = 0; //records time of last rising edge
uint32_t periodSums = 0; //sums up the period of all cycles included
uint32_t numOfRising = 0; //number of rising edges
//uint32_t numOfFalling = 0; //number of falling edges
uint32_t avgPeriod;

//the goal is to find the time between rising edges
//a rising edge is if masked data is NOT 0

for (int i = 0; i < MAXBUF; i++) {
  if (((DataBuffer[i] & mask) != 0) && (prevValue == 0) && (numOfRising > 0)) { //we've had a previous rising edge
    periodSums += prevRiseTime- TimeBuffer[i]; //since we count down, current time goes second
    prevRiseTime = TimeBuffer[i]; //we want to track rising edge for the next time we have a rising edge
    numOfRising++;
  }
  else if (((DataBuffer[i] & mask) != 0) && (prevValue == 0)) { //very first rising edge has no reference, so we don't add to periodSum
    numOfRising++;
    prevRiseTime = TimeBuffer[i]; //we want to track our first rising edge
  }
  prevValue = DataBuffer[i] & mask; //need value to check if we have a rising edge
}
 
  if (numOfRising >= 2) { //we need at least 2 rising edges to calculate period
    avgPeriod = periodSums/(numOfRising - 1); //we don't care about the first rising edge since it didn't generate a period
    return avgPeriod;
  }
  else {
    return 0; //return 0 if not enough data to calculate period
  }
}


// *****Debug_Duty******
// Calculate duty cycle of the recorded data using mask
// Input: mask specifies which bit(s) to observe
// Output: period in percent (0 to 100)
// Period is defined as rising edge (low to high) to the next rising edge.
// High is defined as rising edge (low to high) to the next falling edge.
// Duty cycle is (100*High)/Period
// Return 0 if there is not enough collected data to calculate duty cycle.
uint32_t Debug_Duty(uint32_t mask){
// students write this for Lab 3
// This function should not alter the recorded data.
// AND each recorded data with mask,
//    if nonzero the signal is considered high.
//    if zero, the signal is considered low.
  uint32_t dutyPeriod = Debug_Period(mask); //get period, we need it
  if (dutyPeriod == 0) {
    return 0; //if not enough info to calculate period, then not enough info to calculate duty so return 0
  }

uint32_t prevValue = 0; //records last ANDED value
uint32_t prevRiseTime = 0; //records time of last rising edge
uint32_t pulseSums = 0; //sums up the pulses of all cycles included
uint32_t numOfPulse = 0; //number of rising edges
//uint32_t numOfFalling = 0; //number of falling edges
uint32_t avgPulse;
uint32_t dutyCycle;

//we will average all pulse widths to find duty cycle
//pulse width is defined as the time from rising edge to falling edge
//duty is pulse width (time high) divided by period (time in a cycle) times 100 to make it a percentage

for (int i = 0; i < MAXBUF; i++) {
  if (((DataBuffer[i] & mask) != 0) && (prevValue == 0)) { //are we on a rising edge
    prevRiseTime = TimeBuffer[i]; //track rise time 
  }
  else if (((DataBuffer[i] & mask) == 0) && (prevValue != 0)) { //to fall, we must have risen at some point. we need to find when we last rose
    pulseSums += prevRiseTime - TimeBuffer[i]; //time downwards; this calculate time we were high
    numOfPulse++; //we calculated a pulse
  }
  prevValue = DataBuffer[i] & mask; //keep track of prev data
}

if (numOfPulse > 0) { //we need at least one pulse width (time high) to calculate
  avgPulse = pulseSums/numOfPulse; //average pulse is sum of all pulse widths divided by num of pulse widths recorded
  dutyCycle = (avgPulse * 100)/dutyPeriod; //calcualte duty cycle
  return dutyCycle;
}
return 0;
}

// Lab2 specific debugging code
uint32_t Theperiod;
uint32_t Theduty;
void Dump(void){
  uint32_t out = GPIOB->DOUT31_0&0x0070000; // PB18-PB16 outputs //here we do a bitwise mask to see if high or low on output
  uint32_t in = GPIOB->DIN31_0&0x0F;        // PB3-PB0 inputs //we also track inputs, although we don't actually end up using this data
  uint32_t data = out|in;                   // PB18-PB16, PB3-PB0 //data is defined as keeping track of pins that are outputting and pins that are getting input in range
  uint32_t result = Debug_Dump(data);       // calls your Lab3 function //we take the current state of input and output and put it in data
  if(result == 0){ // 0 means full  //if we attempted to put new data in but returned 0, our buffers are full
    Theperiod = Debug_Period(1<<16);        // calls your Lab3 function and masks the output pin I have to see if I am outputting, and then finds period
    Theduty = Debug_Duty(1<<16); //we need pin 16 
  __asm volatile("bkpt; \n"); // breakpoint here, check period and duty
// observe Theperiod
  }
}





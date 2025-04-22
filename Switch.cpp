/*
 * Switch.cpp
 *
 *  Created on: Nov 5, 2023
 *      Author:
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
// LaunchPad.h defines all the indices into the PINCM table
void Switch_Init(void){
    // write this
  IOMUX->SECCFG.PINCM[PA26INDEX] = 0x00040081; // input, pull
  IOMUX->SECCFG.PINCM[PA31INDEX] = 0x00040081; // input, no pull
  IOMUX->SECCFG.PINCM[PB24INDEX] = 0x00040081; // input, no pull

  IOMUX->SECCFG.PINCM[PB17INDEX] = 0x00040081; // input, no pull
  IOMUX->SECCFG.PINCM[PA27INDEX] = 0x00040081; // input, no pull
  IOMUX->SECCFG.PINCM[PA24INDEX] = 0x00040081; // input, no pull
}
// return current state of switches
uint32_t Switch_In1(void){
    // write this
  uint32_t data = GPIOA->DIN31_0;
  data = data & (1 << 26);
  return data; // return 0; //replace this your code
}

uint32_t P1SHOOT(void){
    // write this
  uint32_t data = GPIOA->DIN31_0;
  data = data & (1 << 31);
  return data; // return 0; //replace this your code
}

uint32_t special1(void) {
  uint32_t data = GPIOA->DIN31_0;
  data = data & (1 << 24);
  return data; // return 0; //replace this your code
}

uint32_t Switch_In2(void){
    // write this
  uint32_t data = GPIOB->DIN31_0;
  data = data & (1 << 17);
  return data; // return 0; //replace this your code
}


uint32_t P2SHOOT(void){
    // write this
  uint32_t data = GPIOA->DIN31_0;
  data = data & (1 << 27);
  return data; // return 0; //replace this your code
}

uint32_t special2(void) {
  uint32_t data = GPIOB->DIN31_0;
  data = data & (1 << 24);
  return data; // return 0; //replace this your code
}



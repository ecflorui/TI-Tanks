// Lab9HMain.cpp
// Runs on MSPM0G3507
// Lab 9 ECE319H
// Krish Singh and Connor McDonald
// TI-Tanks

#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/SlidePot.h"
#include "../inc/DAC5.h"
#include "SmallFont.h"
#include "LED.h"
#include "Switch.h"
#include "Sound.h"
#include "images/images.h"
#include "tank.h"
#include <math.h>
#include "bullet.h"
#include "tools.h" //here's where all helper functions are located
#include "fsm.h" //fsm things are here
#include "LED.h"

#define MAX_BULLETS 3
Bullet bullets1[MAX_BULLETS]; //10 bullets avaliable for each player. if it's a dead, bullet it won't move. if alive, it'll move.
Bullet bullets2[MAX_BULLETS];

extern "C" void __disable_irq(void);
extern "C" void __enable_irq(void);
extern "C" void TIMG12_IRQHandler(void);
using namespace std;


void PLL_Init(void){ // set phase lock loop (PLL)
  // Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}

uint32_t time = 0; //used to go slower than the 30 Hz in the G12 ISR
bool bulletFlag1 = false; //flag for bullet updates
bool bulletFlag2 = false; //flag for bullet updates
bool TG12Flag = false;

//our tanks are 19 by 14 dimension

SlidePot player1SP(0,0); //initializing slidepots for the two players
SlidePot player2SP(0,0); //no input for SP Constructor b/c we only care about raw data, not distance

Tank p1 = Tank(50, 50, 0, //Tank 1
           MiniRed,
           0, 5, 19, 14);
Tank p2 = Tank(100, 50, 0, //Tank 2
           MiniBlue,
           1, 5, 19, 14);



//game engine helper functions are located in tools.h and tools.cpp


// games  engine runs at 30Hz
void TIMG12_IRQHandler(void){uint32_t pos,msg;
      // Acknowledge / debug toggle
    GPIOB->DOUTTGL31_0 = GREEN;
    GPIOB->DOUTTGL31_0 = GREEN;


    if(gameState == STATE_PLAY){
      // primary game engine only in play state
      SlidePotSampler(player1SP);
      SlidePotSampler(player2SP);

      tankMovement(time, 1, p1);
      tankMovement(time, 2, p2);

     p1.TickPower(time);
     p2.TickPower(time);

  // 2) check for activate button press
    if(special2()) p1.TryActivatePower(time);
    if(special1()) p2.TryActivatePower(time);

    if ((time % 200) && (p1.puActive)) {
      LED_Toggle(4);
    }

    if ((time % 200) && (p2.puActive)) {
      LED_Toggle(2);
    }

      queryTank();

      time++;
      TG12Flag = true;
    }
    // NO LCD OUTPUT IN INTERRUPTS

    // Final debug toggle
    GPIOB->DOUTTGL31_0 = GREEN;
  }
  
uint8_t TExaS_LaunchPadLogicPB27PB26(void){
  return (0x80|((GPIOB->DOUT31_0>>26)&0x03));
}



int main(void){ 
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  player1SP.Init(2); //initalizes SlidePot stuff
  player2SP.Init(1); //initalizes SlidePot stuff

  ST7735_InitPrintf(INITR_REDTAB);
  LED_Init();    // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(0,0,&TExaS_LaunchPadLogicPB27PB26);
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  TimerG12_IntArm(2666667,1);
  Switch_Init();
  __enable_irq();
  

  


while(1){
  switch(gameState){
    case STATE_MENU: showMenu();     break;
    case STATE_PLAY: runGameLoop();  break;
    case STATE_END:  showEndScreen();break;
  }
}

}



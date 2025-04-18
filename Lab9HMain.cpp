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

extern "C" void __disable_irq(void);
extern "C" void __enable_irq(void);
extern "C" void TIMG12_IRQHandler(void);
using namespace std;

// ****note to ECE319K students****
// the data sheet says the ADC does not work when clock is 80 MHz
// however, the ADC seems to work on my boards at 80 MHz
// I suggest you try 80MHz, but if it doesn't work, switch to 40MHz
void PLL_Init(void){ // set phase lock loop (PLL)
  // Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}

uint32_t time = 0; //used to go slower than the 30 Hz in the G12 ISR

//our tanks are 19 by 14 dimension

SlidePot player1SP(0,0); //initializing slidepots for the two players
SlidePot player2SP(0,0); //no input for SP Constructor b/c we only care about raw data, not distance

Tank p1 = Tank(50, 100, 0, //Tank 1
           MiniRed,
           1, 3, 19, 14);


Tank p2 = Tank(100, 100, 0, //Tank 2
           RedTank,
           1, 3, 12, 18);

uint32_t M=1;
uint32_t Random32(void){
  M = 1664525*M+1013904223;
  return M;
}
uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}


// games  engine runs at 30Hz
void TIMG12_IRQHandler(void){uint32_t pos,msg;
  if((TIMG12->CPU_INT.IIDX) == 1){ // this will acknowledge
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
// game engine goes here

    // 1) sample slide pot


    uint32_t newData1 = player1SP.In();
    player1SP.Save(newData1);

    // 2) read input switches

  if ((time % 2) == 0) {
      if (Switch_In()) {
        p1.TriVelocity(2);
        p1.Move();
  }
  }

  //p1.SetVelocity(5,0);

    // 3) move sprites

    //move

    // 4) start sounds
    // 5) set semaphore
    time++;
    // NO LCD OUTPUT IN INTERRUPT SERVICE ROUTINES
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
  }
}
uint8_t TExaS_LaunchPadLogicPB27PB26(void){
  return (0x80|((GPIOB->DOUT31_0>>26)&0x03));
}

typedef enum {English, Spanish, Portuguese, French} Language_t;
Language_t myLanguage=English;
typedef enum {HELLO, GOODBYE, LANGUAGE} phrase_t;
const char Hello_English[] ="Hello";
const char Hello_Spanish[] ="\xADHola!";
const char Hello_Portuguese[] = "Ol\xA0";
const char Hello_French[] ="All\x83";
const char Goodbye_English[]="Goodbye";
const char Goodbye_Spanish[]="Adi\xA2s";
const char Goodbye_Portuguese[] = "Tchau";
const char Goodbye_French[] = "Au revoir";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Language_Portuguese[]="Portugu\x88s";
const char Language_French[]="Fran\x87" "ais";
const char *Phrases[3][4]={
  {Hello_English,Hello_Spanish,Hello_Portuguese,Hello_French},
  {Goodbye_English,Goodbye_Spanish,Goodbye_Portuguese,Goodbye_French},
  {Language_English,Language_Spanish,Language_Portuguese,Language_French}
};



int main(void){ 
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  player1SP.Init(); //initalizes SlidePot stuff
  ST7735_InitPrintf(INITR_BLACKTAB);
  LED_Init();    // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(0,0,&TExaS_LaunchPadLogicPB27PB26);
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  TimerG12_IntArm(2666667,1);
  Switch_Init();
  __enable_irq();

  ST7735_FillScreen(0x3467);
  p1.Draw();


while (1) {
  // Check if the SlidePot has new data. This new Sync allows for universal Sync, but will only redraw if need be
  bool newSlidepotData = player1SP.Sync();
  
  // Handle rotation if new slidepot data is available
  if (newSlidepotData) {
    int32_t delta = player1SP.Distance() - player1SP.lastDistance();
    if (delta != 0) {
      p1.rotateIncrement(delta);
    }
  }

  // Only draw when ISR signals it's time
    if (p1.NeedsRedraw()) {
      p1.Draw();
    }
}

}


// use main4 to test sound outputs
int main4(void){ uint32_t last=0,now;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(ADC0,6,0); // ADC1 channel 6 is PB20, TExaS scope
  __enable_irq();
  while(1){
    now = Switch_In(); // one of your buttons
    if((last == 0)&&(now == 1)){
      Sound_Shoot(); // call one of your sounds
    }
    if((last == 0)&&(now == 2)){
      Sound_Killed(); // call one of your sounds
    }
    if((last == 0)&&(now == 4)){
      Sound_Explosion(); // call one of your sounds
    }
    if((last == 0)&&(now == 8)){
      Sound_Fastinvader1(); // call one of your sounds
    }
    // modify this to test all your sounds
  }
}
// ALL ST7735 OUTPUT MUST OCCUR IN MAIN

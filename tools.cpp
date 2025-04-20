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
#include "tools.h"

#define MAX_BULLETS 10
extern uint32_t time;
extern Bullet bullets1[];
extern Bullet bullets2[];
extern bool bulletFlag1;
extern bool bulletFlag2;
extern Tank p1;
extern Tank p2;
extern bool TG12Flag;
extern SlidePot player1SP;
extern SlidePot player2SP;


uint32_t M=1;

void SlidePotSampler(SlidePot &s) {
    uint32_t newData = s.In();
    s.Save(newData);
}

void tankMovement(uint32_t time, uint32_t playerNum, Tank &t) {
    uint32_t buttonSample = 0;
    if (time % 2) {
        (playerNum == 1) ? (buttonSample = Switch_In1()) : (buttonSample = Switch_In2());
        if (buttonSample) {
        t.TriVelocity(2);
        t.Move();
     }
    }
}

void queryTank() {
    if (time % 2) {
    if (P1SHOOT()) 
        bulletFlag1 = true;
    if (P2SHOOT()) 
        bulletFlag2 = true;
    }
}

void bulletUpdate() {
    if (bulletFlag1) {
        p1.Shoot(bullets1, MAX_BULLETS);\
        bulletFlag1 = false;
    }
    if (bulletFlag2) {
        p2.Shoot(bullets2, MAX_BULLETS);
        bulletFlag2 = false;
    }


    if (TG12Flag) {
        p1.TickCooldowns(); //we have a global bullets array
    for (int i = 0; i < MAX_BULLETS; i++) {
    bullets1[i].Move();
    }

    p2.TickCooldowns(); //we have a global bullets array
    for (int i = 0; i < MAX_BULLETS; i++) {
    bullets2[i].Move();
    }

    TG12Flag = false;
    }
}

void rotateUpdate() {
  // Check if the SlidePot has new data. This new Sync allows for universal Sync, but will only redraw if need be
  bool newSlidepotData1 = player1SP.Sync();
  bool newSlidepotData2 = player2SP.Sync();

    // Handle rotation if new slidepot data is available
  if (newSlidepotData1) {
    int32_t delta = player1SP.Distance() - player1SP.lastDistance();
    if (delta != 0) {
      p1.rotateIncrement(delta);
    }
  }

  if (newSlidepotData2) {
    int32_t delta = player2SP.Distance() - player2SP.lastDistance();
    if (delta != 0) {
      p2.rotateIncrement(delta);
    }
  }
}

void displayUpdate() {
     // Only draw when ISR signals it's time
    if (p1.NeedsRedraw())  p1.Draw();
    if (p2.NeedsRedraw()) p2.Draw();
}

uint32_t Random32(void){
  M = 1664525*M+1013904223;
  return M;
}

uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}

int32_t clamper(int pos, int min, int max) {
    if (pos < min) return min;
    if (pos > max) return max;
    return pos;
}

float clamp(float pos, float min, float max) {
    if (pos < min) return min;
    if (pos > max) return max;
    return pos;
}

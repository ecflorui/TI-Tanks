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
#include "walls.h"

using namespace std;

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


//gamestate stuff
int p1SeriesWins = 0;
int p2SeriesWins = 0;
int currentStage   = 0;          
const int MAX_STAGES   = 5;
const int WINS_NEEDED  = 3;   


//different walls
Wall walls[] = {
  {20, 40, 10, 50},
  {60, 100, 30, 10},
};

int NUM_WALLS = sizeof(walls)/sizeof(walls[0]);


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
        t.TriVelocity(1);
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
        p1.Shoot(bullets1, MAX_BULLETS);
        bulletFlag1 = false;
    }
    if (bulletFlag2) {
        p2.Shoot(bullets2, MAX_BULLETS);
        bulletFlag2 = false;
    }


    if (TG12Flag) {
        p1.TickCooldowns(); //we have a global bullets array
    for (int i = 0; i < MAX_BULLETS; i++) {
    bullets1[i].check(p2);
    bullets1[i].check(p1);
    bullets1[i].Move();
    }

    p2.TickCooldowns(); //we have a global bullets array
    for (int i = 0; i < MAX_BULLETS; i++) {
    bullets2[i].check(p1);
    bullets1[i].check(p2);
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
    if (p1.NeedsRedraw())  
    p1.Draw();
    if (p2.NeedsRedraw()) 
    p2.Draw();
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

float clamper(float pos, float min, float max) {
    if (pos < min) return min;
    if (pos > max) return max;
    return pos;
}

bool isCollision() {
    return !(p1.GetX() + 14 <= p2.GetX() || p1.GetX() >= p2.GetX() + 14 ||
             p1.GetY() + 13 <= p2.GetY() || p1.GetY() >= p2.GetY() + 13);
}

bool curWallCollision(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
  for (int i = 0; i < NUM_WALLS; i++) {
    Wall& wall = walls[i];
    bool overlap = !(x + w <= wall.x || x >= wall.x + wall.width || y + (h+1) <= wall.y || y >= wall.y +((wall.height+1)));
    if (overlap) return true;
  }
  return false;
}

// void DrawHealth(const Tank& player1, const Tank& player2) {
//     static int8_t lastHealth1 = -1;
//     static int8_t lastHealth2 = -1;

//     if (player1.GetHealth() != lastHealth1) {
//         lastHealth1 = player1.GetHealth();
//         ST7735_FillRect(0, 0, 40, 8, ST7735_BLACK); // Clear just this text area
//         char buf[8];
//         sprintf(buf, "P1:%d", lastHealth1);
//         ST7735_DrawString(0, 0, buf, ST7735_WHITE);
//     }

//     if (player2.GetHealth() != lastHealth2) {
//         lastHealth2 = player2.GetHealth();
//         ST7735_FillRect(60, 0, 40, 8, ST7735_BLACK); // Clear just this text area
//         char buf[8];
//         sprintf(buf, "P2:%d", lastHealth2);
//         ST7735_DrawString(10, 0, buf, ST7735_WHITE); // (col, row)
//     }
// }

void DrawHealth(const Tank& player1, const Tank& player2) {
  static int8_t lastHealth1 = -1;
  static int8_t lastHealth2 = -1;

  int8_t hp1 = player1.GetHealth();
  int8_t hp2 = player2.GetHealth();

  if (hp1 != lastHealth1 || hp2 != lastHealth2) {
    lastHealth1 = hp1;
    lastHealth2 = hp2;

    const int x = 0;
    const int spacing = 10;

    ST7735_FillRect(x, 0, 10, 160, ST7735_BLACK);

    for (int i = 0; i < hp2; i++) {
      int y = 2 + i * spacing;
      ST7735_DrawBitmap(x + 1, y + 8, heart3, 8, 8);
    }

    for (int i = 0; i < hp1; i++) {
      int y = 160 - 2 - i * spacing - 8;
      ST7735_DrawBitmap(x + 1, y + 8, heart2, 8, 8);
    }
  }
}


void DrawWalls() {
  for (int i = 0; i < NUM_WALLS; i++) {
    Wall& wall = walls[i];
    ST7735_FillRect(wall.x, wall.y, wall.width, wall.height, ST7735_LIGHTGREY);
  }
}
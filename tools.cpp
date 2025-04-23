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
#include "water.h"


using namespace std;

#define MAX_BULLETS 3
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
extern bool deathFlag;
extern bool gameEndFlag;


//gamestate stuff
int p1SeriesWins = 0;
int p2SeriesWins = 0;
int currentStage   = 0;          
const int MAX_STAGES   = 5;
const int WINS_NEEDED  = 3;   





//different walls
Wall walls[30]; //no more than 30 walls
int NUM_WALLS = 0;

Water waters[30]; //no more than 30 waters

int NUM_WATERS = 0;


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
    bullets2[i].check(p2);
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
    bool overlap = !(x + w <= (wall.x) || x >= wall.x + wall.width || y + (h+1) <= wall.y || y >= wall.y +((wall.height+1)));
    if (overlap) return true;
  }
  return false;
}

bool curWaterCollision(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
  for (int i = 0; i < NUM_WATERS; i++) {
    Water& water = waters[i];
    bool overlap = !(x + w <= water.x || x >= water.x + water.width || y + (h+1) <= water.y || y >= water.y +((water.height+1)));
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

void DrawWater() {
  for (int i = 0; i < NUM_WATERS; i++) {
    Water& water = waters[i];
    ST7735_FillRect(water.x, water.y, water.width, water.height, ST7735_BLUE);
  }
}


void generateMap1(uint8_t hp) { //empty
    Wall walls[] = {};
    int NUM_WALLS = 0;

    Water waters[] = {};
    int NUM_WATERS = 0;
    
    p1.SetX(60);
    p1.SetY(140);
    p1.SetHP(hp);

    p2.SetX(60);
    p2.SetY(30);
    p2.SetHP(hp);
}




void generateMap2(uint8_t hp) {
    const uint32_t t = 5;      // wall thickness
    const uint32_t left = 10;  // leave first 10px for health
    const uint32_t width = 128 - left;
    const uint32_t height = 160;

    static const Wall maze[] = {

      { left, 0, width, t},  
      { left, height-t, width, t},  
      { left, 0, t, height},  
      { left+width-t, 0, t, height},  

      // vertical corridors
      {left + 30, 90, t , 70},
      { left + 50, 0, t, 70},
      { left + 80, 88, t, 40},
      { left + 80, 0, t, 20},
      // { left +100,     t,  t,           50         },
      // { left +100,     90, t,           60         },

      // // horizontal corridors
      {left + 60, 128, 78, t },
      {left+30, 50, 46, t},
      {left+100, 50, 20, t},
      // { left,          100,50,          t          },
      // { left+68,       100,50,          t          },
    };


  NUM_WALLS = sizeof(maze)/sizeof(maze[0]);

  for(int i = 0; i < NUM_WALLS; ++i){
    walls[i] = maze[i];
  }


  NUM_WATERS = 0;

  p1.SetX(95); 
  p1.SetY(153); 
  p1.SetHP(hp);
  p2.SetX(20); 
  p2.SetY(20); 
  p2.SetHP(hp);
}



void generateMap3(uint8_t hp) { //the ring

    const uint32_t t = 7;      // wall thickness
    const uint32_t left = 10;  // leave first 10px for health
    const uint32_t width = 128 - left;
    const uint32_t height = 160;

    static const Wall bands[] = {
      {left + 32, 57, t , 53},
      {left + 77, 57, t , 53},
    };

    NUM_WALLS = sizeof(bands)/sizeof(bands[0]);

    static const Water ring[]  = {
      //vertical
      {left + 25, 50, t , 60},
      {left + 85, 50, t , 60},

      //horizontal
      {left + 25, 50, 60 , t},
      {left + 25, 110, 67 , t},
    };
    NUM_WATERS = sizeof(ring)/sizeof(ring[0]);

      for(int i = 0; i < NUM_WATERS; ++i){
    waters[i] = ring[i];
  }

    for(int i = 0; i < NUM_WALLS; ++i){
    walls[i] = bands[i];
  }

    
    p1.SetX(60);
    p1.SetY(140);
    p1.SetHP(hp);

    p2.SetX(60);
    p2.SetY(30);
    p2.SetHP(hp);
}


void generateMap4(uint8_t hp) { //bridges

    const uint32_t t = 10;      // wall thickness
    const uint32_t left = 10;  // leave first 10px for health
    const uint32_t width = 128 - left;
    const uint32_t height = 160;

    Wall bands[] = {
      {left, 65, t, 30},
      {width, 65, t, 30},
    };
    NUM_WALLS = sizeof(bands)/sizeof(bands[0]);

    static const Water river[] = {
      { left, 55, 25, t},  
      { left, 95, 25, t}, 

      {left + 50, 55, 15, t},  
      {left + 50, 95, 15, t}, 

       {left + 90, 55, 38, t},  
      {left + 90, 95, 38, t}, 
    };

     NUM_WATERS = sizeof(river)/sizeof(river[0]);

      for(int i = 0; i < NUM_WATERS; ++i){
    waters[i] = river[i];
  }

    for(int i = 0; i < NUM_WALLS; ++i){
    walls[i] = bands[i];
  }


    p1.SetX(60);
    p1.SetY(140);
    p1.SetHP(hp);

    p2.SetX(60);
    p2.SetY(30);
    p2.SetHP(hp);
}

void generateMap5(uint8_t hp) { //islands
    Wall walls[] = {};
    int NUM_WALLS = 0;

    Water waters[] = {};
    int NUM_WATERS = 0;
    
    p1.SetX(60);
    p1.SetY(140);
    p1.SetHP(5);

    p2.SetX(60);
    p2.SetY(30);
    p2.SetHP(5);
}

void generateMap6(uint8_t hp) { //water shootout
    Wall walls[] = {};
    int NUM_WALLS = 0;

    Water waters[] = {};
    int NUM_WATERS = 0;
    
    p1.SetX(60);
    p1.SetY(140);
    p1.SetHP(hp);

    p2.SetX(60);
    p2.SetY(30);
    p2.SetHP(hp);
}

void generateMap7(uint8_t hp) { //bouncy castle
    Wall walls[] = {};
    int NUM_WALLS = 0;

    Water waters[] = {};
    int NUM_WATERS = 0;
    
    p1.SetX(60);
    p1.SetY(140);
    p1.SetHP(hp);

    p2.SetX(60);
    p2.SetY(30);
    p2.SetHP(hp);
}

void generateMap8(uint8_t hp) { //stranded island
    Wall walls[] = {};
    int NUM_WALLS = 0;

    Water waters[] = {};
    int NUM_WATERS = 0;
    
    p1.SetX(60);
    p1.SetY(140);
    p1.SetHP(hp);

    p2.SetX(60);
    p2.SetY(30);
    p2.SetHP(hp);
}


void generateMap9(uint8_t hp) { //maze 2
    Wall walls[] = {};
    int NUM_WALLS = 0;

    Water waters[] = {};
    int NUM_WATERS = 0;
    
    p1.SetX(60);
    p1.SetY(140);
    p1.SetHP(hp);

    p2.SetX(60);
    p2.SetY(30);
    p2.SetHP(hp);
}

void generateMap10(uint8_t hp) { //ping pong
    Wall walls[] = {};
    int NUM_WALLS = 0;

    Water waters[] = {};
    int NUM_WATERS = 0;
    
    p1.SetX(60);
    p1.SetY(140);
    p1.SetHP(hp);

    p2.SetX(60);
    p2.SetY(30);
    p2.SetHP(hp);
}


void initializeRound(uint8_t hp) {

  __disable_irq();

  uint8_t pickMap = 3; //time %10

  switch (pickMap){
    case 0:
      generateMap1(hp);
      break;

    case 1:
      generateMap2(hp);
      break;

    case 2:
      generateMap3(hp);
      break;
      
    case 3:
      generateMap4(hp);
      break;
    
    case 4:
      generateMap5(hp);
      break;
      
    case 5:
      generateMap6(hp);
      break;

    case 6:
      generateMap7(hp);
      break;
      
    case 7:
      generateMap8(hp);
      break;

    case 8:
      generateMap9(hp);
      break;
      
    case 9:
      generateMap10(hp);
      break;

    default:
      generateMap1(hp);
      break;
  }

  ST7735_FillScreen(0x3467);

  p1.SetAlive(1);
  p1.SetAlive(1);

  p1.Draw();
  p2.Draw();

  DrawWalls();
  DrawWater();

  __enable_irq();
}

// void nextStage(uint8_t w) {

//   __disable_irq();
//   ST7735_FillScreen(ST7735_BLACK);

//   if (w == 0) {
//     //ST7735_DrawString("PLAYER RED LOST"); //placeholder
//     Clock_Delay1ms(500);
//     p2SeriesWins++;
//     if (p2SeriesWins == WINS_NEEDED) {
//       gameEndFlag = true;
//     }
//   }

//   if (w == 1) {

//     //ST7735_DrawString("PLAYER BLUE LOST"); //placeholder
//     Clock_Delay1ms(500);
//     p1SeriesWins++;
//     if (p2SeriesWins == WINS_NEEDED) {
//       gameEndFlag = true;
//       return;
//     }
//   }

//   initializeRound();
// }
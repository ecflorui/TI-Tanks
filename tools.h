
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


class Tank;
class Bullet;

void SlidePotSampler(SlidePot &s);
void tankMovement(uint32_t time, uint32_t playerNum, Tank &t);
void queryTank(void);
void bulletUpdate();
void rotateUpdate();
void displayUpdate();
int32_t clamper(int pos, int min, int max);
float clamper(float pos, float min, float max);
bool isCollision();
bool curWallCollision(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
void DrawHealth(const Tank& player1, const Tank& player2); 

uint32_t Random32(void);
uint32_t Random(uint32_t n);

void DrawWalls();



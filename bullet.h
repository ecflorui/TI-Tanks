// bullet.h

#ifndef BULLET_H
#define BULLET_H

#include "tank.h"
#include "tools.h"

#include <cstdint>
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
#include <math.h>
#include "images/images.h"

class Bullet {
private:
  float x, y;
  float vx, vy;
  bool active;
  int32_t time; //we want the bullets to dissapear
  bool wasDrawn = false;


public:
  Bullet();
  void Init(int32_t startX, int32_t startY, float angleDeg, int32_t speed = 4, int32_t ttlFrames = 300);
  void Move();
  void Draw();
  void Erase();
  bool IsActive() const;
  void Deactivate();
  float getx();
  float gety();
  void check(Tank&);
};

#endif 
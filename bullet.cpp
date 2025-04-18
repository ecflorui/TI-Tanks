
#include "../inc/ST7735.h"
#include <math.h>
#include "tank.h"
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
#include "bullet.h"

Bullet::Bullet() : x(0), y(0), vx(0), vy(0), active(false), time(0) {} //initialize to false, dead bullet

void Bullet::Init(int32_t startX, int32_t startY, float angle, int32_t speed, int32_t f) {
  float rad = angle * (3.14159265f / 180.0f); //convert to radians
  vx = (int32_t)(cosf(rad) * speed);
  vy = (int32_t)(-sinf(rad) * speed);
  x = startX;
  y = startY;
  time = f;
  active = true;
}

void Bullet::Move() {
  if (!active) return;

  if (time <= 296) Erase(); //get rid of old

  x += vx;
  y += vy;
  time--; //less time, will die eventually at every movement

  if (time > 296) return; //avoid intercollision

  // kill bullet
  if (x < 0 || x > 126 || y < 0 || y > 158 || time <= 0) {
    active = false;
  } else {
    Draw();
  }
}

void Bullet::Draw() {
  ST7735_FillRect(x, y, 2, 2, ST7735_YELLOW);
}

void Bullet::Erase() {
  ST7735_FillRect(x, y, 2, 2, 0x3467);
}

bool Bullet::IsActive() const {
  return active;
}

void Bullet::Deactivate() {
  active = false;
}

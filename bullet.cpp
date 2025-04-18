
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

Bullet::Bullet() : x(0), y(0), vx(0), vy(0), active(false), time(0) {}

void Bullet::Init(int32_t startX, int32_t startY, float angle, int32_t speed, int32_t f) {
  float rad = angle * (3.14159265f / 180.0f);
  vx = cosf(rad) * speed;
  vy = -sinf(rad) * speed;
  x = static_cast<float>(startX);
  y = static_cast<float>(startY);
  time = f;
  active = true;
}

void Bullet::Move() {
  if (!active) return;

  if (time <= 296) Erase();

  x += vx;
  y += vy;
  time--;

  if (time > 296) return;

  if (x < 0 || x > 126 || y < 0 || y > 158 || time <= 0) {
    active = false;
  } else {
    Draw();
  }
}

void Bullet::Draw() {
  ST7735_FillRect(static_cast<int>(x), static_cast<int>(y), 2, 2, ST7735_YELLOW);
}

void Bullet::Erase() {
  ST7735_FillRect(static_cast<int>(x), static_cast<int>(y), 2, 2, 0x3467);
}


bool Bullet::IsActive() const {
  return active;
}

void Bullet::Deactivate() {
  active = false;
}

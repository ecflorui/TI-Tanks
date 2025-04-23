
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
#include "walls.h"


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

  // 1) erase old
  if (time <= 116) Erase();

  if (time % 100) //dont want to update too quickly, every 3 s
  DrawWater(); //so that bullets go over 

  // 2) HORIZONTAL step + bounce
  x += vx;
  for (int i = 0; i < NUM_WALLS; i++) {
    const Wall &w = walls[i];
    bool hit = !(x+2 <= w.x   || x   >= w.x+w.width ||
                 y+2 <= w.y   || y   >= w.y+w.height);
    if (hit) {
      // step back, flip, step forward
      x -= vx;
      vx = -vx;
      x += vx;
      break;
    }
  }

  // 3) VERTICAL step + bounce
  y += vy;
  for (int i = 0; i < NUM_WALLS; i++) {
    const Wall &w = walls[i];
    bool hit = !(x+2 <= w.x   || x   >= w.x+w.width ||
                 y+2 <= w.y   || y   >= w.y+w.height);
    if (hit) {
      y -= vy;
      vy = -vy;
      y += vy;
      break;
    }
  }

  // 4) advance life & maybe draw
  time--;
  if (time > 116) return;

  if (x < 10 || x > 126 || y < 0 || y > 161 || time <= 0) {
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
   if (active) Erase();   
  active = false;
}

float Bullet::getx() {
  return x;
  }
float Bullet::gety() {
  return y;
  }

void Bullet::check(Tank& t) {
      if (!IsActive()) return;

    int32_t left = t.GetX();
    int32_t right = t.GetX() + 19;
    int32_t top = t.GetY() - 14;
    int32_t bottom = t.GetY();

    // Simple AABB (Axis-Aligned Bounding Box) check
    if ((x >= left && x <= right && y >= top && y <= bottom) && (time < 115))  {
        t.TakeDamage();
        Deactivate();
    }
}
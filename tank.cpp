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
#include "LED.h"

extern uint32_t time;
extern Tank p1;
extern Tank p2;

Tank::Tank(int32_t startX, int32_t startY, int32_t startAngle,
           const uint16_t* sprite, 
           uint8_t num, uint8_t hp, uint32_t width, uint32_t height) {
    x = startX;
    y = startY; 
    angle = startAngle;
    vx = 0; 
    vy = 0; 
    spriteImage = sprite;
    playerNum = num;
    health = hp;
    shotCooldown = 0;
    dashCooldown = 0;
    alive = true;
    needUpdate = true;


    this->width = width;
    this->height = height;

    //for internal use
    fx = (float)startX;
    fy = (float)startY;
    fvx = 0;
    fvy = 0;


    for (int i = 0; i < width * height; i++) {
    rotatedTank[i] = sprite[i];


    //power up stuff
}
    puType      = PU_None;
    puReady     = false;
    puActive    = false;
    puLoadStart = 0;         
    puEffStart  = 0;


    Rotate(0);
}

// at top of tank.cpp (or in tank.h):
static constexpr uint32_t LOAD_TICKS   = 7 * 30;   // 7 s @30 Hz
static constexpr uint32_t EFFECT_TICKS = 10 * 30;  // 10 s @30 Hz

void Tank::TickPower(uint32_t now) {
    // 1) Kick off the load timer
    if (!puReady && !puActive && puLoadStart == 0) {
        puLoadStart = now;
    }

    // 2) Finish loading after 7 s → power is ready
    if (!puReady && !puActive && now - puLoadStart >= LOAD_TICKS) {
        puReady = true;

        srand(now * x + y);
        puType  = static_cast<Power>((rand()  % 3) + 1);

        // turn on the “ready” LED for this player
        if (playerNum == 0) LED_On(4);  // PA17
        else                LED_On(2);  // PA15

    }

    // 3) Expire the effect after 10 s
    if (puActive && now - puEffStart >= EFFECT_TICKS) {
        puActive    = false;
        puType      = PU_None;
        magnitude   = 1;            // revert speed boost

        // turn off the LED
        if (playerNum == 0) LED_Off(4);
        else                LED_Off(2);


        // immediately begin loading the next power-up
        puLoadStart = now;
        puReady     = false;
    }
}

void Tank::powerDisable() {
puType = PU_None;
    puReady = false;
    puActive = false;
    puLoadStart = 0;
    puEffStart = 0;
}

void Tank::TryActivatePower(uint32_t now) {
  if(puReady && !puActive) {
    puReady     = false;
    puActive    = true;
    puEffStart  = now;
    // apply effect immediately
    power_sound();
    switch(puType) {
      case PU_Shield:
        break;
      case PU_Speed:
        magnitude = 2;
        break;
      case PU_Bullet:
        break;
      default: break;
    }
  }
}

void Tank::Draw() {
    ST7735_DrawBitmap(x, y, rotatedTank, width, height);
    needUpdate = false;
}

void Tank::Erase() {
    ST7735_FillRect(x, y - height, width, height, 0x3467);
}

void Tank::MarkForRedraw() {
    needUpdate = true;
}

bool Tank::NeedsRedraw() {
    return needUpdate;
}

void Tank::Move() {
    float old_fx = fx;
    float old_fy = fy;

    fx += fvx;
    fy += fvy;

    // Clamp
    fx = clamper(fx, 10.0f, 114.0f);
    fy = clamper(fy, 12.0f, 162.0f);

    // Convert to integer
    int next_x = (int)(fx + 0.5f);
    int next_y = (int)(fy + 0.5f);

    // Check collision with new position
    x = next_x;
    y = next_y;
    if (isCollision() || curWallCollision(next_x, next_y - height, width, height) || curWaterCollision(next_x, next_y - height, width, height)) { //check if theres a collision between a tank or a currently generated wall
        // Revert both float and int after colliding cuz we dont want movement
        fx = old_fx;
        fy = old_fy;
        x = (int)(fx + 0.5f);
        y = (int)(fy + 0.5f);
        return;
    }

    ST7735_FillRect(old_fx, old_fy - height, width, height, 0x3467);//updated erase() function for faster magnitudes
    needUpdate = true;
}


void Tank::Dash() {
    if (CanDash()) {
        x += vx * 5;
        y += vy * 5;
        dashCooldown = 0; //change later, testing & development
        needUpdate = true;
    }
}

void Tank::SetVelocity(int32_t vx, int32_t vy) {
    this->vx = vx;
    this->vy = vy;
}


void Tank::rotateSprite(float degrees) {
    const int W = width;
    const int H = height;
    const float cx = W / 2.0f;
    const float cy = H / 2.0f;

    float theta = degrees * (3.14159265f / 180.0f); // convert to radians
    float cosT = cosf(theta);
    float sinT = sinf(theta);

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            // translate destination pixel to origin-centered coords
            float dx = x - cx;
            float dy = y - cy;

            // inverse rotate to find source pixel
            float srcX =  cosT * dx + sinT * dy + cx;
            float srcY = -sinT * dx + cosT * dy + cy;

            int srcXi = (int)(srcX + 0.5f);
            int srcYi = (int)(srcY + 0.5f);

            uint16_t color = 0x3467; // background/fill color

            if (srcXi >= 0 && srcXi < W && srcYi >= 0 && srcYi < H) {
                color = spriteImage[srcYi * W + srcXi]; // nearest neighbor
            }

            rotatedTank[y * W + x] = color;
        }
    }
}

void Tank::Rotate(int32_t changeAngle) { //i need to change this later, understand Valvanos lab lecture for this
    angle = (angle + changeAngle) % 360;
    if (angle < 0) angle += 360;

    rotateSprite((float)angle);
    needUpdate = true;
}

void Tank::rotateIncrement(int32_t delta) {
    if (delta > 0) {
      Rotate(rotationStep);  // CW
    } else {
      Rotate(-rotationStep); // CCW
    }
}

void Tank::TriVelocity() {
    float radians = angle * (3.14159265f / 180.0f);
    
    float dx = cosf(radians);
    float dy = -sinf(radians);

    // Normalize (optional if you're using unit circle but safe)
    float len = sqrtf(dx*dx + dy*dy);
    if (len != 0) {
        dx /= len;
        dy /= len;
    }

    fvx = dx * magnitude;
    fvy = dy * magnitude;
}

void Tank::TakeDamage() {
    if(puActive && puType==PU_Shield) return; //no need to take damage
    if (health > 0) {
        health--;
    }
    if (health == 0) {
        alive = false;
    }
}

bool Tank::CanShoot() {
    return shotCooldown == 0;
}

bool Tank::CanDash() {
    return dashCooldown == 0;
}

void Tank::TickCooldowns() { //function to decrease using some TimerISR
    if (shotCooldown > 0) 
        shotCooldown--;
    if (dashCooldown > 0) 
        dashCooldown--;
}

int32_t Tank::GetX() const { return x; }
int32_t Tank::GetY() const { return y; }
int32_t Tank::GetVX() const { return vx; }
int32_t Tank::GetVY() const { return vy; }
int32_t Tank::GetAngle() const { return angle; }
uint8_t Tank::GetHealth() const { return health; }
uint8_t Tank::GetPlayer() const { return playerNum; }
uint32_t Tank::GetThreshold() {return threshold;}

void Tank::SetX(int32_t px ) { x = px; fx = px; }
void Tank::SetY(int32_t py) { y = py; fy = py; }
void Tank::SetHP(uint8_t hp) { health = hp;}

bool Tank::IsAlive() const { return alive; }

void Tank::SetAlive(bool state) {
    alive = state;
    needUpdate = true;
}

void Tank::SetAngle(float deg) {
    angle = deg;
}

void Tank::Shoot(Bullet bullets[], int maxBullets) {
    if (!CanShoot()) return; //make sure cool down is gone :O

    for (int i = 0; i < maxBullets; i++) { //look at all avaliable bullets
        if (!bullets[i].IsActive()) { //are they active?
            int32_t centerX = x + (width /2); //launch from center
            int32_t centerY = y - height / 2;  

            int speed = 2;
            if(puActive && puType==PU_Bullet) speed *= 2;

            bullets[i].Init(centerX, centerY, (float)angle, speed, 120);  // speed has default 2, life is 4s (120 with 30 hz G12 clock going)
            //300 (10s) life for bullet for testing purposes currently, change this back. customizable for potential powerup?
            if(puActive && puType==PU_Bullet) shotCooldown = 30;
            else shotCooldown = 45; // 1 second cooldown given 30 hz, i may adjust this later
            shoot_sound();
            break;
        }
    }
}


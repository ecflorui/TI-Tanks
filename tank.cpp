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

Tank::Tank(int32_t startX, int32_t startY, int32_t startAngle,
           const uint16_t* sprite, const uint16_t* blank,
           uint8_t num, uint8_t hp, uint32_t width, uint32_t height) {
    x = startX;
    y = startY; 
    angle = startAngle;
    vx = 0; 
    vy = 0; 
    spriteImage = sprite;
    blankImage = blank;
    playerNum = num;
    health = hp;
    shotCooldown = 0;
    dashCooldown = 0;
    alive = true;
    needUpdate = true;
    this->width = width;
    this->height = height;
}

void Tank::Draw() {
    ST7735_DrawBitmap(x, y, spriteImage, width, height);
    needUpdate = false;
}

void Tank::Erase() {
    ST7735_DrawBitmap(x, y, blankImage, width, height);
}

void Tank::MarkForRedraw() {
    needUpdate = true;
}

bool Tank::NeedsRedraw() {
    return needUpdate;
}

void Tank::Move() {
    x += vx;
    y += vy;
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
    const int W = 12;
    const int H = 18;
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

            uint16_t color = ST7735_BLACK; // background/fill color

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

void Tank::TakeDamage() {
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
int32_t Tank::GetAngle() const { return angle; }
uint8_t Tank::GetHealth() const { return health; }
uint8_t Tank::GetPlayer() const { return playerNum; }
bool Tank::IsAlive() const { return alive; }

void Tank::SetAlive(bool state) {
    alive = state;
    needUpdate = true;
}
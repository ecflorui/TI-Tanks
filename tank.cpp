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
#include "images/images.h"

Tank::Tank(int32_t startX, int32_t startY, int32_t startAngle,
           const uint16_t* sprite, const uint16_t* blank,
           uint8_t num, uint8_t hp) {
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
}
#ifndef TANK_H
#define TANK_H

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

struct Tank {
    private:
        int32_t x, y; //position 
        int32_t vx, vy; //velocities
        int32_t angle; //direction of tank
        uint32_t width;
        uint32_t height;

        uint8_t health; //0 to n health of player
        uint32_t shotCooldown; //wait time
        uint32_t dashCooldown;//wait time

        uint8_t playerNum;
        bool needUpdate; //flag to update screen

        bool alive; //dead or alive
        const uint16_t* spriteImage;
        const uint16_t* blankImage;

    public:
        Tank(int32_t startX, int32_t startY, int32_t startAngle,
         const uint16_t* sprite, const uint16_t* blank,
          uint8_t num, uint8_t hp, uint32_t width, uint32_t height);

        void Draw();
        void Erase();
        void MarkForRedraw();
        bool NeedsRedraw();

        void Move();
        void Dash();
        void SetVelocity(int32_t vx, int32_t vy);
        void Rotate(int32_t angleChange);

        void TakeDamage();
        bool CanShoot();
        bool CanDash();
        void TickCooldowns();

        int32_t GetX() const;
        int32_t GetY() const;
        int32_t GetAngle() const;
        uint8_t GetHealth() const;
        uint8_t GetPlayer() const;
        bool IsAlive() const;

        void SetAlive(bool state);
};

#endif 


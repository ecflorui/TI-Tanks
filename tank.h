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
#include "bullet.h"



struct Tank {
    private:
        int32_t x, y; //position 
        int32_t vx, vy; //velocities

        //for internal use
        float fx, fy;     // float position
        float fvx, fvy;   // float velocity
        
        int32_t angle; //direction of tank
        uint32_t width;
        uint32_t height;

        uint8_t health; //0 to n health of player
        uint32_t shotCooldown; 
        uint32_t dashCooldown;
    

        uint8_t playerNum;
        bool needUpdate; //flag to update screen

        bool alive; //dead or alive
        const uint16_t* spriteImage;
        uint16_t rotatedTank[19 * 14]; //19 * 14 is default size
        
        //tweakables
        uint32_t threshold = 100; //threshold for min accepted data change for slidepot (probably a better idea to put this in SlidePot.cpp but its not that serious)
        const int32_t rotationStep = 15; //angle step due to detected change
    

    public:
        Tank(int32_t startX, int32_t startY, int32_t startAngle,
         const uint16_t* sprite,
          uint8_t num, uint8_t hp, uint32_t width, uint32_t height);

        void Draw();
        void Erase();
        void MarkForRedraw();
        bool NeedsRedraw();

        void Move();
        void Dash();
        void SetVelocity(int32_t vx, int32_t vy);
        void Rotate(int32_t angleChange);
        void rotateSprite(float degrees);
        void rotateIncrement(int32_t delta);
        void TriVelocity();
        uint32_t magnitude = 1;

        void TakeDamage();
        bool CanShoot();
        bool CanDash();
        void TickCooldowns();

        int32_t GetX() const;
        int32_t GetY() const;
        int32_t GetAngle() const;
        uint8_t GetHealth() const;
        uint8_t GetPlayer() const;
        int32_t GetVX() const;
        int32_t GetVY() const;
        bool IsAlive() const;

        void SetAlive(bool state);
        void SetAngle(float);
        uint32_t GetThreshold();
        void Shoot(Bullet bullets[], int maxBullets);

        void SetX(int32_t px);
        void SetY(int32_t py);
        void SetHP(uint8_t hp);

        //power up setup 

        enum Power { PU_None=0, PU_Shield, PU_Speed, PU_Bullet } puType;
        bool  puReady;         // this state means that power up can be used
        bool  puActive;        // power up is active for the player
        uint32_t puLoadStart;  // time when load began
        uint32_t puEffStart;   // time when effect began

        void TickPower(uint32_t now);
        void TryActivatePower(uint32_t now);

        static constexpr uint32_t LOAD_TICKS   =  7*30;    // 7 seconds @30 Hz = 210 ticks
        static constexpr uint32_t EFFECT_TICKS = 10*30;    // 10 seconds @30 Hz = 300 ticks

        void powerDisable(void);
};

#endif 
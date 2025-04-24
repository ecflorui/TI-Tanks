// Sound.cpp
// Implements sample playback via SysTick and 12-bit DAC

#include <cmath>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "Sound.h"
#include "sounds.h"          // extern const uint8_t shoot[], hit[]; extern const uint32_t shootLength, hitLength;
#include "DAC.h"             // 12-bit DAC interface

// CPU clock frequency (Hz)
#ifndef SYSCLK_HZ
#define SYSCLK_HZ 80000000u
#endif

static const uint8_t *soundPtr = nullptr;
static uint32_t        soundLen = 0;
static uint32_t        soundIdx = 0;
static bool            playing  = false;

// One-time setup: configure 12-bit DAC and prepare SysTick (disabled)
void Sound_Init(void) {
    // Initialize DAC
    DAC_Init();

    // Prepare SysTick (disabled until clip queued)
    SysTick->CTRL = 0;
    SysTick->LOAD = 1 - 1;
    SysTick->VAL  = 0;

    // Set SysTick interrupt priority (lowest urgency)
    SCB->SHP[1] = (SCB->SHP[1] & ~0xC0000000) | (0 << 30);

    // Enable global interrupts
    __enable_irq();
}

// Immediately stop playback and silence output
void Sound_Stop(void) {
    playing      = false;
    DAC_Out(0);
    SysTick->CTRL = 0;
}

// Internal: start SysTick at given period (in CPU ticks)
static void Sound_Start(uint32_t periodTicks) {
    SysTick->LOAD = periodTicks - 1;
    SysTick->VAL  = 0;
    // Enable SysTick: core clock, interrupt, counter
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk
                  | SysTick_CTRL_TICKINT_Msk
                  | SysTick_CTRL_ENABLE_Msk;
    playing       = true;
}

// Queue shoot sound: play for exactly 1 second
void shoot_sound(void) {
    soundPtr = shoot;
    soundLen = sizeof(shoot);
    soundIdx = 0;
    Sound_Start(SYSCLK_HZ / soundLen);
}

// Queue hit sound: play for exactly 1 second
void hit_sound(void) {
    soundPtr = explosion;
    soundLen = sizeof(explosion);
    soundIdx = 0;
    Sound_Start(SYSCLK_HZ / soundLen);
}

void power_sound(void) {
    soundPtr = highpitch;
    soundLen = sizeof(highpitch);
    soundIdx = 0;
    Sound_Start(SYSCLK_HZ / soundLen);
}

// ISR: output one sample, advance index, stop when done
extern "C" void SysTick_Handler(void) {
    if (!playing || !soundPtr) return;

    // Output next sample
    DAC_Out(soundPtr[soundIdx++]);

    if (soundIdx >= soundLen) {
        // Playback complete
        playing = false;
        DAC_Out(0);
        SysTick->CTRL = 0;
    }
}
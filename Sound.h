
// Sound.h
#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>

// Initialize the Systick (sample interrupt off until you queue a clip)
void Sound_Init(void);

// Immediately stop whatever’s playing
void Sound_Stop(void);

// Queue one of our two clips
void shoot_sound(void);
void hit_sound(void);
void power_sound(void);

#endif // SOUND_H

// fsm.h
#ifndef FSM_H
#define FSM_H

#include <stdint.h>
#include "../inc/ST7735.h"
#include "Switch.h"
#include "tank.h"
#include "bullet.h"
#include "tools.h"   // rotateUpdate, bulletUpdate, displayUpdate, DrawHealth, DrawWalls
#include "walls.h"

// Localization
typedef enum { English, Spanish, Portuguese, French } Language_t;
extern Language_t      myLanguage;
extern const char     *Phrases[7][4];

// FSM states
typedef enum { STATE_MENU, STATE_PLAY, STATE_END } GameState_t;
extern GameState_t     gameState;

// Persistent match data
extern uint8_t         initialLives;
extern uint16_t        winsP1, winsP2;

// Tanks live in main
extern Tank            p1, p2;


// Screen routines
void showMenu(void);
void runGameLoop(void);
void showEndScreen(void);

#endif // FSM_H
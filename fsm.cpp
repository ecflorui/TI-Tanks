// fsm.cpp
#include "fsm.h"
#include "../inc/Clock.h"    // Clock_Delay1ms
#include "../inc/SlidePot.h" // SlidePotSampler if you switch to pot‐based lives
#include "Switch.h"
#include "tank.h" // placement new for Tank re‐init
#include "tools.h"
#include "bullet.h"
#include <cstdint>

extern Bullet bullets1[];
extern Bullet bullets2[];

extern Tank p1;
extern Tank p2;

#define MAX_BULLETS 3


Language_t myLanguage = English;



const char Ready_English[]    = "READY";
const char Ready_Spanish[]    = "LISTO";
const char Ready_Portuguese[] = "PRONTO";
const char Ready_French[]     = "PRÊT";


const char Set_English[]    = "SET";
const char Set_Spanish[]    = "PREPARADO";
const char Set_Portuguese[] = "VAI";
const char Set_French[]     = "PRÊTS";


const char Begin_English[]    = "FIGHT!";
const char Begin_Spanish[]    = "\xA1LUCHA!";         // ¡LUCHA!
const char Begin_Portuguese[] = "LUTE!";
const char Begin_French[]     = "COMBAT !";


const char Player_English[]    = "Player";
const char Player_Spanish[]    = "Jugador";
const char Player_Portuguese[] = "Jogador";
const char Player_French[]     = "Joueur";


const char Wins_English[]    = "Wins";
const char Wins_Spanish[]    = "Victorias";
const char Wins_Portuguese[] = "Vitórias";
const char Wins_French[]     = "Victoires";


const char Again_English[]    = "Play Again? \n(Y/N)\n(L/R)";
const char Again_Spanish[]    = "\xBFJugar otra vez?\n(S/N)\n(L/R)";   // ¿Jugar otra vez?
const char Again_Portuguese[] = "Jogar novamente?\n(S/N)\n(L/R)";
const char Again_French[]     = "Rejouer?\n(O/N)\n(L/R)";

const char Language_English[]="English ";
const char Language_Spanish[]="Espa\xA4ol ";
const char Language_Portuguese[]="Portugu\x88s";
const char Language_French[]="Fran\x87" "ais ";


const char *Phrases[7][4] = {
    {Ready_English, Ready_Spanish, Ready_Portuguese, Ready_French},
    {Set_English, Set_Spanish, Set_Portuguese, Set_French},
    {Begin_English, Begin_Spanish, Begin_Portuguese, Begin_French},
    {Player_English, Player_Spanish, Player_Portuguese, Player_French},
    {Wins_English, Wins_Spanish, Wins_Portuguese, Wins_French},
    {Again_English, Again_Spanish, Again_Portuguese, Again_French},
    {Language_English,Language_Spanish,Language_Portuguese,Language_French}};

GameState_t gameState = STATE_MENU;
uint8_t initialLives = 5;
uint16_t winsP1 = 0, winsP2 = 0;

// Menu strings (mutable arrays so we can pass to ST7735_OutString without cast)
static char playAgainStr[4][16] = {"Play Again", "Jugar otra vez",
                                   "Jogar novamente", "Rejouer"};
static char returnMenuStr[4][16] = {"Main Menu", "Menu Principal",
                                    "Menu Principal", "Menu Principal"};

void showMenu(void) {
 winsP1 = 0; winsP2 = 0;
uint8_t lives = 5;
uint8_t lang = 0;
  ST7735_FillScreen(0x0000); // Black background
  ST7735_SetCursor(2, 0);
  ST7735_OutString((char *)"TI Tanks");
  ST7735_SetCursor(2, 1);
  ST7735_OutString((char *)"Singh-McDonald");
  ST7735_SetCursor(2, 4);
  ST7735_OutString((char *)"Set Lang. (L):");

  ST7735_SetCursor(2, 7);
  ST7735_OutString((char *)"Set Lives (R):");
  ST7735_SetCursor(2, 10);
  ST7735_OutString((char *)"Both Players");
  ST7735_SetCursor(2, 11);
  ST7735_OutString((char *)"Hold (L) to START");
  while (1) {

    Clock_Delay1ms(150); // debounce delay

    uint32_t sw1 = Switch_In1();
    uint32_t sw2 = Switch_In2();
    uint32_t sw5 = special1();
    uint32_t sw6 = special2();

    // Cycle through languages
    if (sw1 && !sw2 || sw2 && !sw1) {
      lang = (lang + 1) % 4;
      ST7735_SetCursor(4, 5);
      ST7735_OutString((char *)Phrases[6][lang]); // language name
      ST7735_SetCursor(1, 11);
    }

    // Increase lives (max 5)
    if (sw6 && !sw5 || sw5 && !sw6) {
      lives = (lives % 7) + 1;

      ST7735_SetCursor(4, 8);
      ST7735_OutUDec(lives);
      ST7735_SetCursor(1, 11);
    }

    // Confirm both to start
    if (sw1 && sw2) {
      myLanguage = (Language_t)lang;
      initialLives = lives;
      gameState = STATE_PLAY;
      return;
    }
  }
}



void runGameLoop(void) {
      __disable_irq();
    ST7735_FillScreen(0x0000);
      ST7735_SetCursor(3, 7);
      ST7735_OutString((char *)Phrases[0][myLanguage]);
      Clock_Delay1ms(1000);
      ST7735_FillScreen(0x0000);
      ST7735_SetCursor(3, 7);
      ST7735_OutString((char *)Phrases[1][myLanguage]);
      Clock_Delay1ms(1000);
      ST7735_FillScreen(0x0000);
      ST7735_SetCursor(3, 7);
      ST7735_OutString((char *)Phrases[2][myLanguage]);
      Clock_Delay1ms(1000);

 initializeRound(initialLives);

  while (gameState == STATE_PLAY) {
    rotateUpdate();
    bulletUpdate();
    displayUpdate();
    DrawHealth(p1, p2);
    if (p1.GetHealth() == 0 || p2.GetHealth() == 0) {
      if (p1.GetHealth() == 0)
        winsP2++;
      else
        winsP1++;
      gameState = STATE_END;
    }
  }
}

void showEndScreen(void) {
  ST7735_FillScreen(0x0000);
  ST7735_SetCursor(0, 0);
  ST7735_OutString((char *)Phrases[4][myLanguage]);
ST7735_SetCursor(0, 1); 
 ST7735_OutString((char *)"P1: ");
  ST7735_OutUDec(winsP1);
  ST7735_SetCursor(0, 2);
  ST7735_OutString((char *)"P2: ");
  ST7735_OutUDec(winsP2);
  ST7735_SetCursor(0, 4);
   ST7735_OutString((char *)Phrases[5][myLanguage]);

   for (int i = 0; i < MAX_BULLETS; i++) {
    bullets1[i].Deactivate();
    bullets2[i].Deactivate();
   }

   p1.powerDisable(); //get rid of power ups
  p2.powerDisable();

   Clock_Delay1ms(1000);


  while (1) {
    uint32_t sw1 = Switch_In1();
    uint32_t sw2 = Switch_In2();
    uint32_t sw5 = special1();
    uint32_t sw6 = special2();


    if (sw1 && sw2 || sw2 && !sw1) {
      gameState = STATE_PLAY;
      return;
    }
    if (sw6 && !sw5 || sw5 && !sw6) {
      gameState = STATE_MENU;
      return;
    }
  }
}
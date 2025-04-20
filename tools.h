
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
#include "tank.h"
#include <math.h>
#include "bullet.h"


class Tank;

void SlidePotSampler(SlidePot &s);
void tankMovement(uint32_t time, uint32_t playerNum, Tank &t);
void queryTank(void);
void bulletUpdate();
void rotateUpdate();
void displayUpdate();

uint32_t Random32(void);
uint32_t Random(uint32_t n);




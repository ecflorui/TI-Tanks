/* SlidePot.cpp
 * Students put your names here
 * Modified: put the date here
 * 12-bit ADC input on ADC1 channel 5, PB18
 */
#include <ti/devices/msp/msp.h>
#include "../inc/Clock.h"
#include "../inc/SlidePot.h"
#include <math.h>
#define ADCVREF_VDDA 0x000
#define ADCVREF_INT  0x200


void SlidePot::Init(uint32_t playerNum){
  ADC1->ULLMEM.GPRCM.RSTCTL = 0xB1000003;
  ADC1->ULLMEM.GPRCM.PWREN = 0x26000001;
  Clock_Delay(24);
  ADC1->ULLMEM.GPRCM.CLKCFG = 0xA9000000;
  ADC1->ULLMEM.CLKFREQ = 7;
  ADC1->ULLMEM.CTL0 = 0x03010000;
  ADC1->ULLMEM.CTL1 = 0x00000000;
  ADC1->ULLMEM.CTL2 = 0x00000000;
  ADC1->ULLMEM.SCOMP0 = 0;
  ADC1->ULLMEM.CPU_INT.IMASK = 0;


  if (playerNum == 1) 
    memIndex = 0;
  else 
    memIndex = 1;
}

uint32_t SlidePot::In(void){
  // Set the channel before starting ADC conversion
  if (memIndex == 0) {
    ADC1->ULLMEM.MEMCTL[0] = 5; // PB18 for player 1
  } else {
    ADC1->ULLMEM.MEMCTL[0] = 4; // PB17 for player 2
  }

  ADC1->ULLMEM.CTL0 |= 0x00000001;             // enable conversions
  ADC1->ULLMEM.CTL1 |= 0x00000100;             // start ADC
  while((ADC1->ULLMEM.STATUS & 0x01) == 0x01) {} // wait for complete

  return ADC1->ULLMEM.MEMRES[0]; // always use MEMRES[0] since we set MEMCTL[0]
}


// constructor, invoked on creation of class
// m and b are linear calibration coefficents
SlidePot::SlidePot(uint32_t m, uint32_t b){
   slope = m;
   offset = b;
   lastData = 0;
}

void SlidePot::Save(uint32_t n){ 
    if (abs((int)n -(int)data) > 50) {
    lastData = data;
    data = n;
    flag = 1;
    }
}
uint32_t SlidePot::Convert(uint32_t n){
  return 1537 * n/4096 + 248;
}
// do not use this function in final lab solution
// it is added just to show you how SLOW floating point in on a Cortex M0+
float SlidePot::FloatConvert(uint32_t input){
  return (0.375156009*input + 247.5787398)/1000;
}

bool SlidePot::Sync(void){
   if (flag) {
    flag = 0;
    return true;  // Data was updated
  }
  return false;   // No new data
}


uint32_t SlidePot::Distance(void){  // return distance value (0 to 2000), 0.001cm
   return data;
}

uint32_t SlidePot::lastDistance(void){  // return distance value (0 to 2000), 0.001cm
   return lastData;
}

uint32_t SlidePot::GetOffset(void){  // return distance value (0 to 2000), 0.001cm
   return offset;
}



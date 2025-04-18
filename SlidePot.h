// SlidePot.h
// Runs on MSPM0
// Provide functions that initialize ADC1 channel 5, PB18 and use a slide pot to measure distance
// Created: 12/26/2024
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#ifndef SLIDEPOT_H
#define SLIDEPOT_H
#include <stdint.h>

class SlidePot{ private:
  uint32_t data;     // raw ADC value
  int32_t flag;      // 0 if data is not valid, 1 if valid
  uint32_t distance; // distance in 0.001cm
  uint32_t lastData;
// distance = (slope*data+offset)/4096
  uint32_t slope;    // calibration coeffients
  uint32_t offset;
  uint32_t memIndex; //specifies which index in MEMCTL the slidepot is, had to add this
public:
  SlidePot(uint32_t m, uint32_t b); // initialize slide pot
  void Init(uint32_t);
  uint32_t In(void);                // return last ADC sample value (0 to 4095)
  void Save(uint32_t n);            // save ADC, set semaphore
  bool Sync(void);                  // wait for semaphore
  uint32_t Convert(uint32_t n);     // convert ADC to raw sample
  float FloatConvert(uint32_t n);   // do not use this function
  uint32_t Distance(void);          // return last distance value (0 to 2000), 0.001cm
  uint32_t lastDistance(void);
  uint32_t GetOffset(void);
};
#endif

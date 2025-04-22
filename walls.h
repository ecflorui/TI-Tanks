// walls.h
#pragma once
#include <cstdint>

struct Wall {
  int32_t x, y, width, height;
};

extern Wall walls[];
extern int   NUM_WALLS;

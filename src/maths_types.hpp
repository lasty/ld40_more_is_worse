#pragma once

#include <cstdint>

struct vec2
{
  float x;
  float y;
};


struct vec3
{
  float x;
  float y;
  float z;
};

struct rect
{
  vec2 position;
  vec2 size;
};

struct col4
{
  col4(float r, float g, float b, float a);
  // col4(const std::string &hex);

  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};


struct mat4
{
  float elements[4][4];
};

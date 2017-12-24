
#include "utils.hpp"

#include <ctime>

Random::Random()
: mt_rand(time(0))
{
}


int Random::Int(int min, int max)
{
  std::uniform_int_distribution<int> distribution(min, max);
  return distribution(mt_rand);
}


float Random::Float(float min, float max)
{
  std::uniform_real_distribution<float> distribution(min, max);
  return distribution(mt_rand);
}


vec2 Random::Position(vec2 min, vec2 max)
{
  return {Float(min.x, max.x), Float(min.y, max.y)};
}


col4 Random::Colour()
{
  return {Float(0.0f, 1.0f), Float(0.0f, 1.0f), Float(0.0f, 1.0f), 1.0f};
}


col4 Random::ColourVarying(col4 colour)
{
  float variance = 0.15f;
  colour.r += Float(-variance, variance);
  colour.g += Float(-variance, variance);
  colour.b += Float(-variance, variance);
  return colour;
}


int Random::Percent()
{
  return Int(1, 100);
}

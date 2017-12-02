#pragma once

#include "maths_types.hpp"

struct Player
{
  vec2 position{200.0f, 200.0f};
  vec2 velocity{0.0f, 0.0f};
};


struct GameState
{
  bool running = false;
  bool debug_enabled = true;

  float wallclock = 0.0f;


  Player player;
};

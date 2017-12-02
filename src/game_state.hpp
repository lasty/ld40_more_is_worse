#pragma once

#include "maths_types.hpp"

#include <map>
#include <string>
#include <vector>


struct Item
{
  bool alive = true;

  vec2 position;
  float radius;
  col4 colour;

  bool colliding = false;

  std::string name;
};


struct Player
{
  vec2 position{200.0f, 200.0f};
  vec2 velocity{0.0f, 0.0f};
  float radius = 30.0f;


  std::map<int, Item> KeyBindInventory;
};


struct GameState
{
  bool running = false;
  bool debug_enabled = true;

  float wallclock = 0.0f;

  bool drop_mode = false;


  Player player;

  std::vector<Item> world_items;

  Item* closest_item = nullptr;
};

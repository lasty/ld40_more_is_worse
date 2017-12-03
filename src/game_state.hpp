#pragma once

#include "maths_types.hpp"

#include <map>
#include <string>
#include <vector>

enum class Active_Type
{
  none,
  passive,
  limited_use,
  toggle,
  hold_down,
};

enum class Item_Type
{
  none,
  command,
  health,
  gun
};

struct Item
{
  bool alive = true;

  Active_Type activate = Active_Type::none;
  Item_Type type = Item_Type::none;

  vec2 position{0.0f, 0.0f};
  float radius = 5.0f;
  col4 colour{0.5f, 0.5f, 0.5f, 1.0f};

  bool colliding = false;

  float cooldown = 0.0f;

  std::string name = "Uninitialized Item!";
};


struct Health
{
  int current;
  int max;
};


struct Player
{
  vec2 position{200.0f, 200.0f};
  vec2 velocity{0.0f, 0.0f};
  float radius = 30.0f;

  Health health{100, 100};

  std::map<int, Item> KeyBindInventory;
};


enum class Monster_Type
{
  none,
  dummy,
  melee,
  shooter
};


struct Monster
{
  bool alive = true;

  Monster_Type type = Monster_Type::none;

  vec2 position{0.0f, 0.0f};
  vec2 velocity{0.0f, 0.0f};

  Health health{10, 10};

  std::string name = "Uninitialized monster!";
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


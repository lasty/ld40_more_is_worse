#pragma once

#include "maths_types.hpp"

#include <map>
#include <string>
#include <vector>

#include "items.hpp"


struct Projectile
{
  vec2 position{0.0f, 0.0f};
  vec2 velocity{0.0f, 0.0f};
  int damage;
  float radius;

  float ttl = 0.0f;
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

  vec2 direction{1.0f, 0.0f};

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
  float radius = 5.0f;

  Health health{0, 0};

  std::string name = "Uninitialized monster!";
};


struct GameState
{
  bool running = false;
  bool debug_enabled = true;

  float wallclock = 0.0f;

  bool drop_mode = false;

  vec2 mouse_position{0.0f, 0.0f};

  Player player{};

  std::vector<Item> world_items;
  std::vector<Projectile> world_projectiles;
  std::vector<Monster> world_monsters;

  Item* closest_item = nullptr;
  Item* mouseover_item = nullptr;
  Monster* mouseover_monster = nullptr;
};

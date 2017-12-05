#pragma once


#include <vector>

#include "utils.hpp"


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

  Item_Type type = Item_Type::none;

  vec2 position{0.0f, 0.0f};
  float radius = 5.0f;
  col4 colour{0.5f, 0.5f, 0.5f, 1.0f};

  bool colliding = false;

  void AddCooldown(float c);
  bool has_cooldown = false;
  float cooldown = 0.0f;
  float cooldown_max = 0.0f;

  void AddLimitedUses(int n);
  bool has_limited_uses = false;
  int uses_left = 0;

  bool CanActivate() const;
  void UseActivation();


  //todo passive, toggle, push to activate

  void CreateCommand(std::string c);
  //bool is_command;
  std::string command;

  void CreateHealing(int amount);
  //bool is_healing = false;
  int healing_amount = 0;

  void CreateGun(int damage);
  //bool is_gun = false;
  int projectile_damage = 0;


  std::string name = "Uninitialized Item!";
};


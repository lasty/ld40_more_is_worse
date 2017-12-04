
#include "items.hpp"


void Item::AddCooldown(float c)
{
  has_cooldown = true;
  cooldown = 0.0f;
  cooldown_max = c;
}


void Item::AddLimitedUses(int n)
{
  has_limited_uses = true;
  uses_left = n;
}


void Item::CreateCommand(std::string cmd)
{
  type = Item_Type::command;
  command = cmd;
}


void Item::CreateHealing(int amount)
{
  type = Item_Type::health;
  healing_amount = amount;
}


void Item::CreateGun(int damage)
{
  type = Item_Type::gun;
  projectile_damage = damage;
}


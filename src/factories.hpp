#pragma once

#include <vector>

#include "game_types.hpp"


class ItemFactory
{
private:
  std::vector<Item_Type> item_type_list;
  std::vector<Monster_Type> monster_type_list;

public:
  Random random;

  ItemFactory();

  Item GenerateRandomItem();

  Monster GenerateRandomMonster();


  Item GetCommand(std::string what);
};

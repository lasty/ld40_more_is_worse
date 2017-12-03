#pragma once

#include "game_state.hpp"

#include <vector>

#include "utils.hpp"


class ItemFactory
{
private:
  std::vector<Active_Type> active_type_list;
  std::vector<Item_Type> item_type_list;
  std::vector<Monster_Type> monster_type_list;

public:
  Random random;

  ItemFactory();

  Item GenerateRandomItem();

  Monster GenerateRandomMonster();


  Item GetCommand(std::string what);
};
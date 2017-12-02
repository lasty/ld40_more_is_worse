#pragma once

#include <map>
#include <random>
#include <vector>

#include "game_state.hpp"
#include "items.hpp"
#include "maths_types.hpp"
#include "sound.hpp"


class Game
{
public:
  GameState gamestate;
  Sound sound;
  ItemFactory item_factory;


  std::mt19937 mt_rand;
  int item_name_number = 0;

public:
  Game();

  void NewGame();

  void NewPlayer();

  void UpdatePlayer(float dt);
  void UpdateItem(Item& item, float dt);
  void Update(float dt);

  void ProcessKeyInput(int key, bool down);
  void ProcessMouseInput(int button, bool down);

  void PickupItem(int key, Item& item);
  void DropItem(int key, bool down);

  void ActivateCommand(Item& item, bool down);

  void ActivateItem(Item& item, bool down);

  void RemoveDeadItems();

  int RandomInt(int min, int max);
  float RandomFloat(float min, float max);

  vec2 GenerateRandomPosition();
  col4 GenerateRandomColour();

  Item GenerateRandomItem(vec2 position);

  bool Collides(const Player& player, const Item& item);
  bool Collides(const Item& item1, const Item& item2);
};

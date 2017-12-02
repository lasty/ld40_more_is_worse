#pragma once

#include <map>
#include <random>
#include <vector>

#include "game_state.hpp"
#include "maths_types.hpp"
#include "sound.hpp"


class Game
{
public:
  GameState gamestate;
  Sound sound;


  std::mt19937 mt_rand;

public:
  Game();

  void NewGame();

  void Update(float dt);

  void ProcessKeyInput(int key, bool down);
  void ProcessMouseInput(int button, bool down);

  void ActivateItem(Item &item);
  void RemoveDeadItems();

  int RandomInt(int min, int max);
  float RandomFloat(float min, float max);

  vec2 GenerateRandomPosition();
  col4 GenerateRandomColour();

  Item GenerateRandomItem(vec2 position);

  bool Collides(const Player& player, const Item& item);
  bool Collides(const Item& item1, const Item& item2);
};

#pragma once

#include <map>
#include <vector>

#include "game_state.hpp"
#include "maths_types.hpp"
#include "sound.hpp"


class Game
{
public:
  GameState gamestate;
  Sound sound;


public:
  Game();

  void Update(float dt);

  void ProcessKeyInput(int key, bool down);
  void ProcessMouseInput(int button, bool down);


};

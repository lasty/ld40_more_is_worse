
#include "game.hpp"

#include <algorithm>

#include "maths.hpp"
#include "sound.hpp"
#include "to_string.hpp"


Game::Game()
{
}

void Game::Update(float dt)
{
  gamestate.wallclock += dt;
}
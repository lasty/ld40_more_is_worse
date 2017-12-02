
#include "game.hpp"

#include <algorithm>

#include "maths.hpp"
#include "sound.hpp"
#include "to_string.hpp"

#include <SDL.h>


Game::Game()
{
}

void Game::Update(float dt)
{
  gamestate.wallclock += dt;

  gamestate.player.position += (gamestate.player.velocity * dt);
}


void Game::ProcessKeyInput(int key, bool down)
{
  float player_speed = down ? 400 : 0;

  if (key == SDLK_w) gamestate.player.velocity.y = -player_speed;
  if (key == SDLK_s) gamestate.player.velocity.y = player_speed;
  if (key == SDLK_a) gamestate.player.velocity.x = -player_speed;
  if (key == SDLK_d) gamestate.player.velocity.x = player_speed;

  if (down) std::cout << "Input key: '" << SDL_GetKeyName(key) << "'" << std::endl;
}


void Game::ProcessMouseInput(int button, bool down)
{
  if (down) std::cout << "Input mouse button: '" << GetMouseButtonName(button) << "'" << std::endl;
}
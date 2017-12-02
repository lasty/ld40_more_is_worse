
#include "game.hpp"

#include <algorithm>
#include <ctime>
#include <sstream>

#include "maths.hpp"
#include "sound.hpp"
#include "to_string.hpp"
#include "utils.hpp"

#include <SDL.h>


Game::Game()
: mt_rand(time(0))
{
}


void Game::UpdatePlayer(float dt)
{
  gamestate.player.position += (gamestate.player.velocity * dt);

  for (auto& it : gamestate.player.KeyBindInventory)
  {
    UpdateItem(it.second, dt);
  }
}


void Game::UpdateItem(Item& item, float dt)
{
  if (item.cooldown > 0.0f)
  {
    item.cooldown -= dt;
    if (item.cooldown < 0.0f) item.cooldown = 0.0f;
  }
}


void Game::Update(float dt)
{
  gamestate.wallclock += dt;

  UpdatePlayer(dt);

  gamestate.closest_item = nullptr;
  for (auto& item : gamestate.world_items)
  {
    UpdateItem(item, dt);

    if (Collides(gamestate.player, item))
    {
      if (gamestate.closest_item == nullptr)
      {
        gamestate.closest_item = &item;
      }
      else
      {
        if (distance(gamestate.player.position, item.position) < distance(gamestate.player.position, gamestate.closest_item->position))
        {
          gamestate.closest_item = &item;
        }
      }
    }
  }
}


void Game::ProcessKeyInput(int key, bool down)
{
  float player_speed = down ? 400 : 0;

  if (key == SDLK_w) gamestate.player.velocity.y = -player_speed;
  if (key == SDLK_s) gamestate.player.velocity.y = player_speed;
  if (key == SDLK_a) gamestate.player.velocity.x = -player_speed;
  if (key == SDLK_d) gamestate.player.velocity.x = player_speed;

  if (key == SDLK_ESCAPE and down) gamestate.running = false;

  if (key == SDLK_BACKSPACE and down)
  {
    gamestate.drop_mode = not gamestate.drop_mode;
    std::cout << (gamestate.drop_mode ? "DROP MODE" : "Pickup Mode") << std::endl;
  }

  //if down, and not a reserved key
  if (down and key != SDLK_w and key != SDLK_s and key != SDLK_a and key != SDLK_d and key != SDLK_ESCAPE and key != SDLK_BACKSPACE)
  {
    std::cout << "Input key: '" << SDL_GetKeyName(key) << "'  "
              << (gamestate.drop_mode ? "DROP MODE" : "Pickup Mode") << std::endl;

    auto it = gamestate.player.KeyBindInventory.find(key);

    if (not gamestate.drop_mode)
    {
      if (it == gamestate.player.KeyBindInventory.end())
      {
        if (gamestate.closest_item != nullptr)
        {
          //add to inv
          Item i = *gamestate.closest_item;

          std::cout << "Picked up item '" << i.name << "'  - Bound to key  " << SDL_GetKeyName(key) << std::endl;

          gamestate.player.KeyBindInventory.insert({key, i});
          gamestate.closest_item->alive = false;
        }
        else
        {
          std::cout << "Nothing in inventory slot " << SDL_GetKeyName(key) << std::endl;
        }
      }
      else
      {
        Item& item = it->second;

        ActivateItem(item);
      }
    }
    else //Drop mode
    {
      if (it == gamestate.player.KeyBindInventory.end())
      {
        std::cout << "Noting in that inventory slot to drop  " << SDL_GetKeyName(key) << std::endl;
        gamestate.drop_mode = false;
      }
      else
      {
        Item i = it->second;

        std::cout << "Dropping item " << i.name << std::endl;

        i.position = gamestate.player.position + vec2{RandomFloat(-20, 20), RandomFloat(-20, 20)};

        gamestate.world_items.push_back(i);
        gamestate.closest_item = nullptr;

        gamestate.player.KeyBindInventory.erase(it);
        gamestate.drop_mode = false;
      }
    }
  }
}


void Game::ProcessMouseInput(int button, bool down)
{
  if (down) std::cout << "Input mouse button: '" << GetMouseButtonName(button) << "'" << std::endl;
}


void Game::ActivateItem(Item& item)
{
  if (item.cooldown > 0.0f)
  {
    std::cout << "'" << item.name << "' is recharging (" << item.cooldown << " seconds)" << std::endl;
  }
  else
  {
    std::cout << "Activate item  '" << item.name << "'  !!!  " << std::endl;
    item.cooldown = 5.0f;
  }
}


void Game::RemoveDeadItems()
{
  gamestate.closest_item = nullptr;

  remove_if_inplace(gamestate.world_items, [](auto& i) { return not i.alive; });
}


int Game::RandomInt(int min, int max)
{
  std::uniform_int_distribution<int> distribution(min, max);
  return distribution(mt_rand);
}


float Game::RandomFloat(float min, float max)
{
  std::uniform_real_distribution<float> distribution(min, max);
  return distribution(mt_rand);
}


vec2 Game::GenerateRandomPosition()
{
  return {RandomFloat(0, 1000), RandomFloat(0, 1000)};
}


col4 Game::GenerateRandomColour()
{
  return {RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f),
    RandomFloat(0.0f, 1.0f), 1.0f};
}


Item Game::GenerateRandomItem(vec2 position)
{
  Item i;
  i.position = position;
  i.radius = 50.0f;
  i.colour = GenerateRandomColour();

  std::stringstream ss;
  ss << "Item_" << item_name_number;
  i.name = ss.str();
  item_name_number++;

  return i;
}


void Game::NewGame()
{
  gamestate.wallclock = 0.0f;
  gamestate.running = true;
  gamestate.debug_enabled = false;
  gamestate.world_items.clear();

  item_name_number = 0;

  for (int i = 0; i < 10; i++)
  {
    Item item = GenerateRandomItem(GenerateRandomPosition());

    gamestate.world_items.push_back(item);
  }

  Player new_player;
  gamestate.player = new_player;
}


bool Game::Collides(const Player& player, const Item& item)
{
  float dist = distance(player.position, item.position);
  float radii = player.radius + item.radius;
  return dist <= radii;
}


bool Game::Collides(const Item& item1, const Item& item2)
{
  float dist = distance(item1.position, item2.position);
  float radii = item1.radius + item2.radius;
  return dist <= radii;
}

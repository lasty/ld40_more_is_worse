
#include "game.hpp"

#include <algorithm>
#include <cassert>
#include <sstream>

#include "maths.hpp"
#include "sound.hpp"
#include "to_string.hpp"
#include "utils.hpp"

#include <SDL.h>


Game::Game()
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

  std::cout << "Input key: '" << SDL_GetKeyName(key) << "'  "
            //<< (gamestate.drop_mode ? "DROP MODE" : "Pickup Mode")
            << (down ? "(Down)" : "(Up)")
            << std::endl;

  if (not gamestate.drop_mode)
  {
    auto it = gamestate.player.KeyBindInventory.find(key);
    if (it == gamestate.player.KeyBindInventory.end())
    {
      if (gamestate.closest_item != nullptr)
      {
        if (down)
        {
          PickupItem(key, *gamestate.closest_item);
          gamestate.closest_item = nullptr;
        }
      }
      else
      {
        std::cout << "Nothing in inventory slot " << SDL_GetKeyName(key) << std::endl;
      }
    }
    else
    {
      Item& item = it->second;

      ActivateItem(item, down);
    }
  }
  else //Drop mode
  {
    DropItem(key, down);
  }
}


void Game::PickupItem(int key, Item& item)
{
  std::cout << "Picked up item '" << item.name << "'  - Bound to key  " << SDL_GetKeyName(key) << std::endl;
  assert(not key_exists(gamestate.player.KeyBindInventory, key));

  gamestate.player.KeyBindInventory.insert({key, item});
  item.alive = false;
}


void Game::DropItem(int key, bool down)
{
  auto it = gamestate.player.KeyBindInventory.find(key);
  if (it == gamestate.player.KeyBindInventory.end())
  {
    std::cout << "Noting in that inventory slot to drop  " << SDL_GetKeyName(key) << std::endl;
    gamestate.drop_mode = false;
  }
  else
  {
    Item i = it->second;

    if (i.type == Item_Type::command)
    {
      if (i.name == "DROP" and not down)
      {
        gamestate.drop_mode = false;
      }
      else
      {
        std::cout << "Cannot drop or rebind commands";
      }
      return;
    }

    std::cout << "Dropping item " << i.name << std::endl;

    i.position = gamestate.player.position + vec2{RandomFloat(-20, 20), RandomFloat(-20, 20)};

    gamestate.world_items.push_back(i);
    gamestate.closest_item = nullptr;

    gamestate.player.KeyBindInventory.erase(it);
    gamestate.drop_mode = false;
  }
}


void Game::ProcessMouseInput(int button, bool down)
{
  if (down) std::cout << "Input mouse button: '" << GetMouseButtonName(button) << "'" << std::endl;
}

void Game::ActivateCommand(Item& item, bool down)
{
  float player_speed = down ? 400 : 0;

  if (item.name == "UP") gamestate.player.velocity.y = -player_speed;
  if (item.name == "DOWN") gamestate.player.velocity.y = player_speed;
  if (item.name == "LEFT") gamestate.player.velocity.x = -player_speed;
  if (item.name == "RIGHT") gamestate.player.velocity.x = player_speed;

  if (item.name == "MENU" and down) gamestate.running = false;

  if (item.name == "DROP")
  {
    gamestate.drop_mode = down;
    std::cout << (gamestate.drop_mode ? "DROP MODE" : "Pickup Mode") << std::endl;
  }
}


void Game::ActivateItem(Item& item, bool down)
{
  if (item.type == Item_Type::command) return ActivateCommand(item, down);

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


Item Game::GenerateRandomItem(vec2 position)
{
  Item i = item_factory.GenerateRandomItem();

  i.position = position;

  return i;
}


void Game::NewPlayer()
{
  gamestate.player = Player{};

  //Setup initial keybinds for WASD and backspace
  auto up = item_factory.GetCommand("UP");
  auto down = item_factory.GetCommand("DOWN");
  auto left = item_factory.GetCommand("LEFT");
  auto right = item_factory.GetCommand("RIGHT");

  PickupItem(SDLK_w, up);
  PickupItem(SDLK_s, down);
  PickupItem(SDLK_a, left);
  PickupItem(SDLK_d, right);

  auto menu = item_factory.GetCommand("MENU");
  auto drop = item_factory.GetCommand("DROP");
  PickupItem(SDLK_ESCAPE, menu);
  PickupItem(SDLK_BACKSPACE, drop);
}


void Game::NewGame()
{
  gamestate.wallclock = 0.0f;
  gamestate.running = true;
  gamestate.debug_enabled = false;
  gamestate.world_items.clear();

  NewPlayer();


  for (int i = 0; i < 100; i++)
  {
    Item item = GenerateRandomItem(random.Position());

    gamestate.world_items.push_back(item);
  }
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

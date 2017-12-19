
#include "game.hpp"

#include <algorithm>
#include <cassert>
#include <sstream>

#include "maths.hpp"
#include "sound.hpp"
#include "to_string.hpp"
#include "utils.hpp"

#include <SDL.h>


constexpr bool DEBUG_INPUT = false;


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

  item.animation_time += dt;
}


void Game::UpdateProjectile(Projectile& projectile, float dt)
{
  projectile.ttl -= dt;
  projectile.position += (projectile.velocity * dt);
}


void Game::UpdateMonster(Monster& monster, float dt)
{
  if (monster.health.current <= 0) monster.alive = false;
  monster.position += (monster.velocity * dt);
}


template<typename ITEM>
ITEM* GetClosest(vec2 position, ITEM* i1, ITEM& i2)
{
  if (i1 == nullptr)
  {
    return &i2;
  }
  else
  {
    if (distance(position, i2.position) < distance(position, i1->position))
    {
      return &i2;
    }
    else
    {
      return i1;
    }
  }
}


void Game::Update(float dt)
{
  gamestate.wallclock += dt;

  UpdatePlayer(dt);

  gamestate.closest_item = nullptr;
  gamestate.mouseover_item = nullptr;

  for (auto& item : gamestate.world_items)
  {
    UpdateItem(item, dt);

    if (Collides(gamestate.player, item))
    {
      gamestate.closest_item = GetClosest(gamestate.player.position, gamestate.closest_item, item);
    }

    if (Collides(gamestate.mouse_position, 20.0f, item.position, item.radius))
    {
      gamestate.mouseover_item = GetClosest(gamestate.mouse_position, gamestate.mouseover_item, item);
    }
  }

  for (auto& projectile : gamestate.world_projectiles)
  {
    UpdateProjectile(projectile, dt);

    for (auto& monster : gamestate.world_monsters)
    {
      if (Collides(projectile, monster))
      {
        monster.health.current -= projectile.damage;
        projectile.ttl = 0.0f;
        std::cout << "projectile hit " << monster.name << " for " << projectile.damage << " damage." << std::endl;
      }
    }
  }

  gamestate.mouseover_monster = nullptr;

  for (auto& monster : gamestate.world_monsters)
  {
    UpdateMonster(monster, dt);

    if (Collides(gamestate.mouse_position, 20.0f, monster.position, monster.radius))
    {
      gamestate.mouseover_monster = GetClosest(gamestate.mouse_position, gamestate.mouseover_monster, monster);
    }
  }
}


void Game::ProcessKeyInput(int key, bool down)
{
  if constexpr (DEBUG_INPUT)
    std::cout << "Input key: '" << GetInputName(key) << "'  "
              << (down ? "(Pressed)" : "(Released)") << std::endl;

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
          gamestate.closest_item = gamestate.mouseover_item = nullptr;
        }
      }
      else
      {
        std::cout << "Nothing in inventory slot " << GetInputName(key) << std::endl;
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
    gamestate.closest_item = gamestate.mouseover_item = nullptr;
  }
}


void Game::ProcessMouseInput(int button, bool down)
{
  if constexpr (DEBUG_INPUT and down)
    std::cout << "Input mouse button: '" << GetInputName(button) << "'" << std::endl;

  ProcessKeyInput(button, down);
}


void Game::ProcessMouseMotion(int x, int y)
{
  gamestate.mouse_position = {float(x), float(y)};

  vec2 player_to_mouse = gamestate.mouse_position - gamestate.player.position;
  if (player_to_mouse.x == 0.0f and player_to_mouse.y == 0.0f) player_to_mouse.x = 1.0f;

  gamestate.player.direction = player_to_mouse;
}


void Game::PickupItem(int key, Item& item)
{
  std::cout << "Picked up item '" << item.name << "'  - Bound to key  " << GetInputName(key) << std::endl;
  assert(not key_exists(gamestate.player.KeyBindInventory, key));

  gamestate.player.KeyBindInventory.insert({key, item});
  item.alive = false;
}


void Game::DropItem(int key, bool down)
{
  auto it = gamestate.player.KeyBindInventory.find(key);
  if (it == gamestate.player.KeyBindInventory.end())
  {
    std::cout << "Noting in that inventory slot to drop  " << GetInputName(key) << std::endl;
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
    gamestate.closest_item = gamestate.mouseover_item = nullptr;

    gamestate.player.KeyBindInventory.erase(it);
    gamestate.drop_mode = false;
  }
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

  if (item.CanActivate())
  {
    if (down)
    {
      std::cout << "Activate item  '" << item.name << "'  !!!  " << std::endl;

      item.UseActivation();

      if (item.type == Item_Type::health)
      {
        std::cout << "Health + " << item.healing_amount << std::endl;
        gamestate.player.health.current += item.healing_amount;
      }

      if (item.type == Item_Type::gun)
      {
        ShootProjectile(gamestate.player.position, gamestate.player.direction, item);
      }
    }
  }
}


void Game::RemoveDeadItems()
{
  gamestate.closest_item = gamestate.mouseover_item = nullptr;
  gamestate.mouseover_monster = nullptr;

  remove_if_inplace(gamestate.world_items, [](auto& i) { return not i.alive; });

  remove_if_inplace(gamestate.world_projectiles, [](auto& p) { return p.ttl <= 0.0f; });

  remove_if_inplace(gamestate.world_monsters, [](auto& m) { return not m.alive; });
}


void Game::ShootProjectile(vec2 position, vec2 direction, Item& item)
{
  Projectile p;
  p.position = position;
  p.velocity = normalize(direction) * 800.0f;
  p.radius = 20.0f;
  p.damage = item.projectile_damage;

  p.ttl = 2.0f;

  gamestate.world_projectiles.push_back(p);
}


Item Game::GenerateRandomItem(vec2 position)
{
  Item i = item_factory.GenerateRandomItem();

  i.position = position;

  return i;
}


Monster Game::GenerateRandomMonster(vec2 position)
{
  Monster m = item_factory.GenerateRandomMonster();

  m.position = position;

  return m;
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
  gamestate.world_projectiles.clear();
  gamestate.world_monsters.clear();

  gamestate.closest_item = gamestate.mouseover_item = nullptr;
  gamestate.mouseover_monster = nullptr;

  NewPlayer();

  constexpr int num_items = 15;
  constexpr int num_monsters = 10;

  for (int i = 0; i < num_items; i++)
  {
    Item item = GenerateRandomItem(random.Position());

    gamestate.world_items.push_back(item);
  }

  for (int i = 0; i < num_monsters; i++)
  {
    Monster monster = GenerateRandomMonster(random.Position());

    gamestate.world_monsters.push_back(monster);
  }
}


bool Game::Collides(const vec2& p1, float r1, const vec2& p2, float r2)
{
  float dist = distance(p1, p2);
  float radii = r1 + r2;
  return dist <= radii;
}


bool Game::Collides(const Player& player, const Item& item)
{
  return Collides(player.position, player.radius, item.position, item.radius);
}


bool Game::Collides(const Item& item1, const Item& item2)
{
  return Collides(item1.position, item1.radius, item2.position, item2.radius);
}

bool Game::Collides(const Player& player, const Monster& monster)
{
  return Collides(player.position, player.radius, monster.position, monster.radius);
}

bool Game::Collides(const Projectile& projectile, const Monster& monster)
{
  return Collides(projectile.position, projectile.radius, monster.position, monster.radius);
}

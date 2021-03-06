#pragma once

#include <map>
#include <random>
#include <vector>

#include "factories.hpp"
#include "game_types.hpp"
#include "items.hpp"
#include "maths_types.hpp"
#include "sound.hpp"
#include "utils.hpp"


class Game
{
public:
  GameState gamestate;
  Sound sound;
  ItemFactory item_factory;

  Random random;

  struct {
    bool flag1 = false;
    bool flag2 = false;
  } debug;


public:
  Game();

  void NewGame();

  void NewPlayer();

  void UpdatePlayer(float dt);
  void UpdateItem(Item& item, float dt);
  void UpdateProjectile(Projectile& projectile, float dt);
  void UpdateMonster(Monster& monster, float dt);

  void Update(float dt);

  void ProcessKeyInput(int key, bool down);
  void ProcessMouseInput(int button, bool down);
  void ProcessMouseMotion(int x, int y);

  void PickupItem(int key, Item& item);
  void DropItem(int key, bool down);

  void ActivateCommand(Item& item, bool down);

  void ActivateItem(Item& item, bool down);

  void RemoveDeadItems();

  void ShootProjectile(vec2 position, vec2 direction, Item& item);


  Item GenerateRandomItem(vec2 position);
  Monster GenerateRandomMonster(vec2 position);

  bool Collides(const vec2& p1, float r1, const vec2& p2, float r2);
  bool Collides(const Player& player, const Item& item);
  bool Collides(const Item& item1, const Item& item2);
  bool Collides(const Player& player, const Monster& monster);
  bool Collides(const Projectile& projectile, const Monster& monster);
};


#include "sprites.hpp"


SpriteFactory::SpriteFactory()
{
  sprite_list.emplace("healthkit_base", Sprite{0, 0, 32, 32, 2});
  sprite_list.emplace("healthkit_top", Sprite{0, 0, 32, 32, 3});
  sprite_list.emplace("gun", Sprite{0, 32, 32, 32, 2});
}


const Sprite &SpriteFactory::GetSprite(const std::string &sprite_name) const
{
  return sprite_list.at(sprite_name);
}
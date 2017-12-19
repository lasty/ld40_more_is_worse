
#include "sprites.hpp"

#include <cassert>
#include <cmath>

const Sprite &Animation::GetFrame(float anim_timer) const
{
  float cycle = fmod(anim_timer / cycle_time, 1.0f);
  int which_frame = cycle * frames.size();

  return frames.at(which_frame);
}


SpriteFactory::SpriteFactory()
{
  sprite_list.emplace("healthkit_top", Sprite{0, 0, 32, 32, 4});
  sprite_list.emplace("healthkit_base", Sprite{0, 0, 32, 32, 3});
  sprite_list.emplace("healthkit_shadow", Sprite{0, 0, 32, 32, 2});


  sprite_list.emplace("gun_top", Sprite{0, 32, 32, 32, 4});
  sprite_list.emplace("gun_base", Sprite{0, 32, 32, 32, 3});
  sprite_list.emplace("gun_shadow", Sprite{0, 32, 32, 32, 2});


  Animation star;
  star.frames.push_back(Sprite{480, 32 * 0, 32, 32, 3});
  star.frames.push_back(Sprite{480, 32 * 1, 32, 32, 3});
  star.frames.push_back(Sprite{480, 32 * 2, 32, 32, 3});
  star.frames.push_back(Sprite{480, 32 * 3, 32, 32, 3});
  star.frames.push_back(Sprite{480, 32 * 4, 32, 32, 3});
  star.frames.push_back(Sprite{480, 32 * 5, 32, 32, 3});
  star.frames.push_back(Sprite{480, 32 * 6, 32, 32, 3});
  star.frames.push_back(Sprite{480, 32 * 7, 32, 32, 3});
  star.frames.push_back(Sprite{480, 32 * 8, 32, 32, 3});
  star.frames.push_back(Sprite{480, 32 * 9, 32, 32, 3});
  star.frames.push_back(Sprite{480, 32 * 10, 32, 32, 3});
  star.frames.push_back(Sprite{480, 32 * 11, 32, 32, 3});
  star.frames.push_back(Sprite{480, 32 * 12, 32, 32, 3});
  star.frames.push_back(Sprite{480, 32 * 13, 32, 32, 3});

  star.cycle_time = 0.8f;

  animation_list.insert({"star", star});
}


const Sprite &SpriteFactory::GetSprite(const std::string &sprite_name) const
{
  return sprite_list.at(sprite_name);
}


const Animation &SpriteFactory::GetAnimation(const std::string &name) const
{
  return animation_list.at(name);
}

#pragma once

#include <map>
#include <string>
#include <vector>


struct Sprite
{
  int x;
  int y;
  int width;
  int height;
  int layer;
};


struct Animation
{
  float cycle_time;
  std::vector<Sprite> frames;

  const Sprite& GetFrame(float anim_timer) const;
};


class SpriteFactory
{
private:
  std::map<std::string, Sprite> sprite_list;

  std::map<std::string, Animation> animation_list;

public:
  SpriteFactory();

  const Sprite &GetSprite(const std::string &sprite_name) const;
  const Animation &GetAnimation(const std::string &name) const;
};
